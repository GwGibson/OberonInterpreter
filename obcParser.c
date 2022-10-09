/* Oberon-S Parser Implementation  */

#include "obcScanner.h"
#include "obcParser.h"
#include "obcCompiler.h"

char progName[MAX_VAR_LEN];

/*inline */ void accept(int expectedSym)
{
  PARSER_PRINT("Parsing symbol: %s, expecting symbol: %s\n", symbols[sym], symbols[expectedSym]);
  #if ERROR_HANDLING
  while (sym == UNKNOWN)
  {
    PARSER_PRINT("Skipping over UNKNOWN token\n");
    nextSym();
  }
  #endif
  sym == expectedSym ? nextSym() : parseError(0);
}

void acceptFinal(int expectedSym)
{
  PARSER_PRINT("Parsing EOF symbol: %s, expecting symbol: %s\n", symbols[sym], symbols[expectedSym]);
  if (sym != expectedSym)
    parseError(2);
  PARSER_PRINT("Parse Successful\n");
}

/*1.  Module -> MODULE Ident ; Block . */
void Module()
{
  accept(MODULEsym);
	strcpy(progName, varBuffer);
  accept(Ident);
  accept(SEMIC);
	enterScope();
  Block(0);
	exitScope();
	insertInstr(hlt, 0, 0);
  acceptFinal(PER);
}

/*2.  Block ->  DeclSeq [BEGIN StatSeq] END Ident */
void Block(int procEntry)
{
	int displacement = 1;
	int saveSymTblPoint = symTblPoint;
	
	/* address of start of block */
	int locCounter0 = locCounter;
	insertInstr(jmp, 0, 0);
  DeclSeq(&displacement);
	/* program/proc entry point */
	symTbl[procEntry].procRecord.entryAddr = locCounter;
	code[locCounter0].addr = locCounter;
	insertInstr(isp, 0, displacement - 1); /* allocate stack space for vars */
	
  if (sym == BEGINsym)
  {
    confirmSym();
    int ret = StatSeq();
		/* RETURN in void procedure */
		if(ret == 1 && symTbl[procEntry].procRecord.resAddr == 0)
			compileError(22);
		/* no RETURN in non-void procedure */
		else if(ret == 0 && symTbl[procEntry].procRecord.resAddr != 0)
			compileError(23);
  }
  accept(ENDsym);
	
	/* checking declaration and end of block name match */
	if(procEntry == 0) /* main program block */
	{
		if(strcmp(progName, varBuffer) != 0)
			compileError(20);
	}
	else if(strcmp(symTbl[procEntry].idName, varBuffer) != 0)
		compileError(21);
  accept(Ident);
	
	/* restore symTblPoint, removes local vars */
	symTblPoint = saveSymTblPoint;
}

/*3.  DeclSeq -> [ CONST  ConstDecls ][ TYPE  TypeDecls ][ VAR  VarDecls ]{ ProcDecl } */
void DeclSeq(int *displacement)
{
  if (sym == CONSTsym)
  {
    confirmSym();
    ConstDecls();
  }
  if (sym == TYPEsym)
  {
    confirmSym();
    TypeDecls();
  }
  if (sym == VARsym)
  {
    confirmSym();
    VarDecls(displacement);
  }
  while (sym == PROCEDUREsym)
    ProcDecl();
	
	#if SYM_TABLE_OUT
		printf("\n***End of declarations tables:***");
		printTbls();
	#endif
}

/*4.  ConstDecls -> ConstDecl { ConstDecl } */
void ConstDecls()
{
  ConstDecl();
  while (sym == Ident)
    ConstDecl();
}

/*5.  ConstDecl -> Ident = ConstExpr  ; */
void ConstDecl()
{
	if(sym == Ident)
		insertIdent(varBuffer, -1, CONSTREC);
	int saveSymTblPoint = symTblPoint;
	accept(Ident);
  accept(EQUAL);
	int value, type;
  ConstExpr(&value, &type);
	symTbl[saveSymTblPoint].constRecord.constVal = value; 
	symTbl[saveSymTblPoint].type = type;
  accept(SEMIC);
}

/*6.  TypeDecls -> TypeDecl { TypeDecl } */
void TypeDecls()
{
  TypeDecl();
  while (sym == Ident)
    TypeDecl();
}

/*7.  TypeDecl -> Ident = Type ; */
void TypeDecl()
{
	if(sym == Ident)
		insertIdent(varBuffer, -1, TYPEREC);
	int saveSymTblPoint = symTblPoint;
  accept(Ident);
  accept(EQUAL);
	int type, typeSize;
  Type(&type, &typeSize);
	symTbl[saveSymTblPoint].type = type;
	typeTbl[typeTblPoint].symTblIndex = saveSymTblPoint;
  accept(SEMIC);
}

/*8. Type -> Ident | ArrayType | RecordType | EnumType */
void Type(int *type, int *typeSize)
{
	*type = NOTYPE;
	*typeSize = 1;
	if(sym == Ident)
	{
		int index = lookupIdent(varBuffer);
		if(index != 0)
		{
			if(symTbl[index].recType != TYPEREC)
				compileError(9);
			*type = symTbl[index].type;
			*typeSize = typeTbl[symTbl[index].type].size;
		}
		confirmSym();
	}
	else if(sym == ARRAYsym)
		ArrayType(type, typeSize);
	else if(sym == RECORDsym)
		RecordType(type, typeSize);
	else if(sym == LPAREN)
		EnumType(type, typeSize);
	else
		parseError(3);
}

/*9. ArrayType -> ARRAY Length { , Length } OF Type */
/*10. Length -> ConstExpr*/
void ArrayType(int *type, int *typeSize)
{
  accept(ARRAYsym);
	int length, indexType, ofType;
  ConstExpr(&length, &indexType);
	int length2;
  while (sym == COMMA)
  {
    confirmSym();
    ConstExpr(&length2, &indexType);
		length *= length2;
  }
  accept(OFsym);
  Type(&ofType, typeSize);
	*typeSize = *typeSize * length;
	insertType("ARRAY", *typeSize, ARRAYREC);
	*type = typeTblPoint;
	typeTbl[typeTblPoint].arrayRecord.indexType = indexType;
	typeTbl[typeTblPoint].arrayRecord.elementType = ofType;
	typeTbl[typeTblPoint].size = length;
}

/*11. RecordType -> RECORD FieldList { ; FieldList } END */
void RecordType(int *type, int *typeSize)
{
	enterScope();
  accept(RECORDsym);
	*typeSize = 0;
	int disp = 1;
  FieldList(typeSize, &disp);
  while (sym == SEMIC)
  {
    confirmSym();
    FieldList(typeSize, &disp);
  }
  accept(ENDsym);
	insertType("RECORD", *typeSize, RECREC);
	typeTbl[typeTblPoint].recordRecord.lastField = symTblPoint;
	typeTbl[typeTblPoint].size = *typeSize ;
	*type = typeTblPoint;
	exitRecScope();
}


/*12. EnumType -> '(' Ident { , Ident } ')' */
void EnumType(int *type, int *typeSize)
{
	/* new type */
	*typeSize = 1;
	insertType("ENUM", 1, ENUMREC);
	*type = typeTblPoint;
	int val = 0;
	accept(LPAREN);
	if(sym == Ident)
	{
		insertIdent(varBuffer, 1, CONSTREC);
		symTbl[symTblPoint].constRecord.constVal = val;
		val++;
		accept(Ident);
		while(sym == COMMA)
		{
			confirmSym();
			if(sym == Ident)
			{
				insertIdent(varBuffer, 1, CONSTREC);
				symTbl[symTblPoint].constRecord.constVal = val;
				val++;
				accept(Ident);
			}
			else
				compileError(9);
		}
	}
	else
		compileError(9);
  
  accept(RPAREN);
	typeTbl[typeTblPoint].enumRecord.lastEnum = symTblPoint;
}

/*13.  VarDecls -> VarDecl { VarDecl } */
void VarDecls(int *displacement)
{
  VarDecl(displacement);
  while (sym == Ident)
    VarDecl(displacement);
}

/*14.  VarDecl -> IdentList : Type ; */
void VarDecl(int *displacement)
{
  int firstEntry, lastEntry;
  IdentList(VARREC, &firstEntry, &lastEntry);
  accept(COLON);
	int type, typeSize;
  Type(&type, &typeSize);
	
	if(typeTbl[type].recType == ENUMREC)
		type = 1;
	
	do{
		symTbl[firstEntry].type = type;
		/* if the new entry is based off a declared RECORD TYPE */
		if(typeTbl[type].recType == RECREC && symTbl[typeTbl[type].symTblIndex].recType == TYPEREC)
		{
			int i, fieldDisp = 1;
			enterScope();
			
			for(i = typeTbl[type].symTblIndex + 1; i <= typeTbl[type].recordRecord.lastField; i++)
			{
				insertIdent(symTbl[i].idName, symTbl[i].type, FIELDREC);
				symTbl[symTblPoint].fieldRecord.fieldAddr = fieldDisp;
				fieldDisp++;
			}
			exitRecScope();
		}

		symTbl[firstEntry].varRecord.varAddr = *displacement;
		*displacement += typeSize;
		firstEntry++;
	}while(firstEntry <= lastEntry);
  accept(SEMIC);
}

/*15. Identlist -> Ident { , Ident } */
void IdentList(int recType, int *first, int *last)
{
	if(sym == Ident)
	{
		insertIdent(varBuffer, -1, recType);
		*first = symTblPoint;
		accept(Ident);
	}
	else
		compileError(10);
  while (sym == COMMA)
  {
    confirmSym();
    if(sym == Ident)
		{
			insertIdent(varBuffer, -1, recType);
			accept(Ident);
		}
		else
			compileError(10);
  }
	*last = symTblPoint;
}

/*16. ProcDecl -> PROCEDURE Ident [ FormalPars ] [ : Ident ] ; ProcBody ; */
void ProcDecl()
{
  accept(PROCEDUREsym);
	int displacement = -2;
	int procPoint;
	if(sym == Ident)
	{
		insertIdent(varBuffer, 0, PROCREC);
		procPoint = symTblPoint;
		accept(Ident);
	}
	else
		compileError(10);
	enterScope();
  if (sym == LPAREN) /* has params */
  {
    confirmSym();
    FormalPars(procPoint, &displacement);
  }
	else
		symTbl[procPoint].procRecord.lastParam = 0;
  if (sym == COLON) /* has return value */
  {
    confirmSym();
		if(sym != Ident)
			compileError(10);
		else
		{
			int index = lookupIdent(varBuffer);
			accept(Ident);
			/* confirm return value is a valid type and set return type */
			if(symTbl[index].recType == TYPEREC)
				symTbl[procPoint].type = symTbl[index].type;
			else
				compileError(9);
			
			symTbl[procPoint].procRecord.resAddr = displacement - typeTbl[symTbl[procPoint].type].size;
		}
  }
  accept(SEMIC);
	/* ProcBody -> Block */
  Block(procPoint);
	insertInstr(ret, 0, 0);
	/* backpatch store for return value for functions */
	if(symTbl[procPoint].procRecord.resAddr != 0)
		code[locCounter - 2].addr = symTbl[procPoint].procRecord.resAddr;
	exitRecScope();
  accept(SEMIC);
}

/*17. FormalPars ->  '(' [ FPSection { ; FPSection } ] ')' */
void FormalPars(int procPoint, int *displacementp)
{
  if (sym == VARsym || sym == Ident)
  {
    FPSection();
    while (sym == SEMIC)
    {
      confirmSym();
      FPSection();
    }
		
  }
	symTbl[procPoint].procRecord.lastParam = symTblPoint;
	addressParams(procPoint, displacementp);
  accept(RPAREN);
}

void addressParams(int procPoint, int *displacementp)
{
	int lastParam = symTbl[procPoint].procRecord.lastParam;
	int paramType;
	
	while(lastParam > procPoint)
	{
		paramType = symTbl[lastParam].type;
		*displacementp = *displacementp - typeTbl[paramType].size;
		symTbl[lastParam].paramRecord.paramAddr = *displacementp;
		lastParam--;
	}
}

/*18. FPSection -> [ VAR ] IdentList : FormalType 
  19. FormalType -> Ident
*/
void FPSection()
{
	int isVar = 0;
  if (sym == VARsym)
	{
		isVar = 1;
    confirmSym();
	}
  int paramPoint, last;
  IdentList(PARAMREC, &paramPoint, &last);
  accept(COLON);
	int type;
	if(sym == Ident)
	{
		int index = lookupIdent(varBuffer);
		if(index != 0)
		{
			if(symTbl[index].recType != TYPEREC)
				compileError(11);
			else
				type = symTbl[index].type;
		}
		
	}
	else
		compileError(10);
	
	while(paramPoint <= symTblPoint)
	{
		symTbl[paramPoint].type = type;
		symTbl[paramPoint].paramRecord.variable = isVar;
		paramPoint++;
	}
	accept(Ident);
}

/*21. FieldList -> [ IdentList : Type ] */
void FieldList(int *typeSize, int *disp)
{
  if (sym == Ident)
  {
		int firstEntry, lastEntry;
    IdentList(FIELDREC, &firstEntry, &lastEntry);
    accept(COLON);
		int type, typeSize2;
    Type(&type, &typeSize2);
		
		do{
			symTbl[firstEntry].type = type;
			symTbl[firstEntry].fieldRecord.fieldAddr = *disp;
			*typeSize += typeSize2;
			*disp += typeSize2;
			firstEntry++;
		}while(firstEntry <= lastEntry);
  }
}

/*22. StatSeq -> Stat { ; Stat } */
int StatSeq()
{
  int ret = Stat();
	if(ret == 1)
	{
		if(sym == SEMIC)
			nextSym();
		if(sym != ENDsym)
			compileWarning(2);
		while(sym != ENDsym)
			nextSym();
	}
	else if(ret > 1)
		return ret;
  while (sym == SEMIC)
  {
		#if SYM_TABLE_OUT
			printf("\n***Start of statement tables:***");
			printTbls();
		#endif
    confirmSym();
    ret = Stat();
		if(ret == 1)
		{
			if(sym == SEMIC)
				nextSym();
			if(sym != ENDsym)
				compileWarning(2);
			while(sym != ENDsym)
				nextSym();
		}
		else if(ret > 1)
			return ret;
  }
	return ret;
}

/*23. Stat -> [ AssignStat | ProcCall | IfStat | WhileStat | RepeatStat */
/*              | ForStat | LoopStat | CaseStat | EXIT | RETURN [ Expr ] ] */
int Stat()
{
  switch(sym)
  {
    case Ident: /* AssignStat & ProcCall start with Ident */
      procOrAssign();
      break;
    case IFsym:
      return IfStat();
      break;
    case WHILEsym:
      WhileStat();
      break;
    case REPEATsym:
      RepeatStat();
      break;
    case FORsym:
      ForStat();
      break;
    case LOOPsym:
      LoopStat();
      break;
    case CASEsym:
      CaseStat();
      break;
    case EXITsym:
      confirmSym();
			insertInstr(jmp, 0, locCounter + 1);
			return locCounter - 1;
      break;
    case RETURNsym:
      confirmSym();
			int type;
      if (exprCheck())
			{
        Expr(&type);
				/* store return value in resAddr on stack - backpatched at end of ProcDecl*/
				insertInstr(sto, 0, -1);
				return 1;
			}
      break;
  }
	return 0;
}

void procOrAssign()
{
	int index = lookupIdent(varBuffer);
	int recType = symTbl[index].recType;
  accept(Ident);
  if (sym == ASSGN || sym == PER || sym == LBRAC) /* Regular assignment */
    AssignStat(index);
	/* ABS and ODD must not be LHS */
	else if(recType == STDPROCREC && (symTbl[index].stdProcRecord.stdProcNum == 1 || symTbl[index].stdProcRecord.stdProcNum == 2))
	{
		compileError(18);
		stdProcCall(symTbl[index].stdProcRecord.stdProcNum);
	}
  else if (sym == LPAREN || (recType == PROCREC || recType == STDPROCREC)) /* ProcCall */
	{
		if(recType == PROCREC)
			ProcCall(index);
		else if(recType == STDPROCREC)
			stdProcCall(symTbl[index].stdProcRecord.stdProcNum);
	}
}

/*24. AssignStat -> Designator := Expr */
/*49. Designator -> Ident { Selector } */
void AssignStat(int indexLHS)
{
	int varAddr = symTbl[indexLHS].varRecord.varAddr;
	int offset = 1, selType = -1;
	int nextIndex = indexLHS;
  while(sym == PER || sym == LBRAC)
		offset += Selector(&nextIndex, &selType);
	
  accept(ASSGN);
	int type;
  Expr(&type);
	
	if(typeTbl[symTbl[indexLHS].type].recType == ENUMREC)
		checkTypes(1, type);
	else if(typeTbl[symTbl[indexLHS].type].recType == ARRAYREC)
		checkTypes(typeTbl[symTbl[indexLHS].type].arrayRecord.elementType, type);
	else if(typeTbl[symTbl[indexLHS].type].recType == RECREC)
		checkTypes(selType, type);
	else
		checkTypes(symTbl[indexLHS].type, type);
	switch(symTbl[indexLHS].recType)
	{
		case VARREC:
			insertInstr(sto, 0, symTbl[indexLHS].varRecord.varAddr + offset - 1);
			break;
		case FIELDREC:
			insertInstr(sto, 0, varAddr + symTbl[indexLHS].fieldRecord.fieldAddr);
			break;
		case PARAMREC:
			if(symTbl[indexLHS].paramRecord.variable == 1)
				insertInstr(stoi, 0, symTbl[indexLHS].paramRecord.paramAddr);
			else
				insertInstr(sto, 0, symTbl[indexLHS].paramRecord.paramAddr);
			break;
		case CONSTREC:
			compileError(15);
			break;
		default:
			compileError(12);
	}
}

/*25. ProcCall -> Ident ( [ ActParams ] | ReadParams | [ ReadLnParams ] | WriteParams | [ WriteLnParams ] ) */
void ProcCall(int procPoint)
{
	int paramLength = 0;
	if(sym == LPAREN)
	{
		confirmSym();
		
		if(sym == RPAREN) /* no params */
			confirmSym();
		else
			ActParams(procPoint, &paramLength);
	}
	if((symTbl[procPoint].procRecord.lastParam != 0) && (paramLength != (symTbl[procPoint].procRecord.lastParam - procPoint)))
		compileError(16);
	
	insertInstr(call, currentLevel - symTbl[procPoint].idLevel, symTbl[procPoint].procRecord.entryAddr);
	insertInstr(isp, 0, -paramLength);
}

void stdProcCall(int stdProcNum)
{
	accept(LPAREN);
	int type;
	switch(stdProcNum)
	{
		case 1: /* ABS */
			Expr(&type);
			if(type != INTTYPE)
				compileError(17);
			insertInstr(iabs, 0, 0);
			accept(RPAREN);
			break;
		case 2: /* ODD */
			Expr(&type);
			if(type != INTTYPE)
				compileError(19);
			insertInstr(lodc, 0, 2);
			insertInstr(imod, 0, 0);
			insertInstr(lodc, 0, 1);
			insertInstr(eq, 0, 0);
			accept(RPAREN);
			break;
		case 3: /* READ */
		case 4: /* READLN */
			do{
				if(sym == Ident)
				{
					/* lookup parameters */
					int index = lookupIdent(varBuffer);
					if(index != 0)
					{
						if(symTbl[index].recType == VARREC) 
						{
							checkTypes(symTbl[index].type, INTTYPE);
							insertInstr(loda, currentLevel - symTbl[index].idLevel, symTbl[index].varRecord.varAddr);
							insertInstr(rdi, 0, 0);
						}
						else if(symTbl[index].recType == PARAMREC)
						{
							checkTypes(symTbl[index].type, INTTYPE);
							if(symTbl[index].paramRecord.variable == 1)
								insertInstr(lod, currentLevel - symTbl[index].idLevel, symTbl[index].varRecord.varAddr);
							else
								insertInstr(loda, currentLevel - symTbl[index].idLevel, symTbl[index].varRecord.varAddr);
							insertInstr(rdi, 0, 0);
						}
						else
							compileError(13);
					}
					else
						compileError(13);
				}
				else
					compileError(13);
				accept(Ident);
				if(sym == COMMA)
					accept(COMMA);
			}while(sym == Ident);
			accept(RPAREN);
			if(stdProcNum == 4)
				insertInstr(rdl, 0, 0);
			break;
		case 5: /* WRITE */
		case 6: /* WRITELN */
			do{
				int type;
				Expr(&type);
				insertInstr( wri, 0, 0);
				if(sym == COMMA)
					accept(COMMA);
			}while(exprCheck());
			accept(RPAREN);
			if(stdProcNum == 6)
				insertInstr(wrl, 0, 0);
			break;
		
	}
}

/*26. ActParams -> '(' [ Expr { ',' Expr } ] ')' */
void ActParams(int procPoint, int *paramLength)
{
	int nextParamPoint = procPoint + 1;
	/* function parameter given an identifier */
	if(symTbl[nextParamPoint].paramRecord.variable == 1)
	{
		if(sym == Ident)
		{
			int index = lookupIdent(varBuffer);
			if(index != 0)
			{
				checkParamTypes(symTbl[nextParamPoint].type, symTbl[index].type);
				if(symTbl[index].recType == VARREC)
					insertInstr(loda, currentLevel - symTbl[index].idLevel, symTbl[index].varRecord.varAddr);		
				else if(symTbl[index].recType == PARAMREC)
					insertInstr(lod, currentLevel - symTbl[index].idLevel, symTbl[index].paramRecord.paramAddr);		
				*paramLength = *paramLength + 1;				
			}
			else 
				compileError(6);

			accept(Ident);
		}
		else
		{
			confirmSym();
			compileError(29);
		}
	}
	/* function parameter given a non-identifier expr */
	else
	{
		int type;
		Expr(&type);
		checkParamTypes(symTbl[nextParamPoint].type, type);
		*paramLength += 1;
	}
	while(sym == COMMA)
	{
		confirmSym();
		nextParamPoint = nextParamPoint + 1;
		/* function parameter given an identifier */
		if(symTbl[nextParamPoint].paramRecord.variable == 1)
		{
			if(sym == Ident)
			{
				int index = lookupIdent(varBuffer);
				if(index != 0)
				{
					checkParamTypes(symTbl[nextParamPoint].type, symTbl[index].type);
					if(symTbl[index].recType == VARREC)
						insertInstr(loda, currentLevel - symTbl[index].idLevel, symTbl[index].varRecord.varAddr);		
					else if(symTbl[index].recType == PARAMREC)
						insertInstr(lod, currentLevel - symTbl[index].idLevel, symTbl[index].paramRecord.paramAddr);	
					*paramLength = *paramLength + 1;	
				}
				else 
					compileError(6);

				accept(Ident);
			}
			else
			{
				confirmSym();
				compileError(29);
			}
		}
		else
		{
			int type;
			Expr(&type);
			checkParamTypes(symTbl[nextParamPoint].type, type);
			*paramLength += 1;
		}
	}
	
  accept(RPAREN);
}

/*33. IfStat -> IF Expr THEN StatSeq { ELSIF Expr THEN StatSeq } [ ELSE StatSeq ] END */
int IfStat()
{
	int ret;
  accept(IFsym);
	int type;
  Expr(&type);
	checkTypes(type, BOOLTYPE);
  accept(THENsym);
	/* conditional jump to elseif, else, or end */
	int jmpcIF = locCounter;
	insertInstr(jmpc, 0, 0);
  ret = StatSeq();
	/* unconditional jump to end */
	int jmpIF = locCounter;
	insertInstr(jmp, 0, 0);
	int isELSIF = 0;
	int endJumps[15];
	int index = 0;
	if(sym == ELSIFsym)
	{
		isELSIF = 1;
		/* if there is an elseif, if jmpc needs to go here */
		code[jmpcIF].addr = locCounter;
		int jmpcPrev;
		while (sym == ELSIFsym)
		{
			confirmSym();
			Expr(&type);
			checkTypes(type, BOOLTYPE);
			/* conditional jump to next elseif, else, or end */
			jmpcPrev = locCounter;
			insertInstr(jmpc, 0, 0);
			accept(THENsym);
			ret = StatSeq();
			/* unconditional jump to end */
			endJumps[index] = locCounter;
			insertInstr(jmp, 0, 0);
			index++;
			code[jmpcPrev].addr = locCounter;
		}
	}
  if (sym == ELSEsym)
  {
		/* if there is no elseif, if jmpc needs to go here */
		if(isELSIF == 0)
			code[jmpcIF].addr = locCounter;
		
    confirmSym();
    ret = StatSeq();
  }
	/* if no elseif or no else, if jmpc needs to go here */
	else if(isELSIF == 0)
		code[jmpcIF].addr = locCounter;
		
	/* all unconditional jumps go here */
	code[jmpIF].addr = locCounter;
	int i;
	for(i = 0; i < index; i++)
		code[endJumps[i]].addr = locCounter;
  accept(ENDsym);
	return ret;
}

/*34. WhileStat -> WHILE Expr DO StatSeq { ELSIF expr DO StatSeq } END */
void WhileStat()
{
  accept(WHILEsym);
  int type;
	int whileStartLoc = locCounter; /* location of expr */
  Expr(&type);
	checkTypes(type, BOOLTYPE);
	int jmpcWhile = locCounter;
	insertInstr(jmpc, 0, 0);
  accept(DOsym);
  int exitJmpLoc = StatSeq();
	insertInstr(jmp, 0, whileStartLoc);
	code[jmpcWhile].addr = locCounter;
	int jmpcElsif;
  while (sym == ELSIFsym)
  {
    confirmSym();
    Expr(&type);
		checkTypes(type, BOOLTYPE);
		jmpcElsif = locCounter;
		insertInstr(jmpc, 0, 0);
    accept(DOsym);
    exitJmpLoc = StatSeq();
		insertInstr(jmp, 0, whileStartLoc);
		code[jmpcElsif].addr = locCounter ;
  }
  accept(ENDsym);
	if(exitJmpLoc > 1)
		code[exitJmpLoc].addr = locCounter;
}

/*35. RepeatStat -> REPEAT StatSeq UNTIL Expr */
void RepeatStat()
{
  accept(REPEATsym);
	int repeatStart = locCounter;
  int exitJmpLoc = StatSeq();
  accept(UNTILsym);
	int type;
  Expr(&type);
	checkTypes(type, BOOLTYPE);
	insertInstr(jmpc, 0, repeatStart);
	if(exitJmpLoc > 1)
		code[exitJmpLoc].addr = locCounter;
}

/*36. ForStat -> FOR Ident := Expr TO Expr [ BY ConstExpr ] DO StatSeq END */
void ForStat()
{
  accept(FORsym);
	int index;
	if(sym == Ident)
	{
		index = lookupIdent(varBuffer);
		if(symTbl[index].recType == VARREC)
			insertInstr(loda, currentLevel - symTbl[index].idLevel, symTbl[index].varRecord.varAddr);
		else if(symTbl[index].recType == PARAMREC)
		{
			if(symTbl[index].paramRecord.variable == 1)
				insertInstr(lod, 0, symTbl[index].paramRecord.paramAddr);
			else
				insertInstr(loda, 0, symTbl[index].paramRecord.paramAddr);
		}
		accept(Ident);
	}
	else
		compileError(10);
  
  accept(ASSGN);
	int type;
  Expr(&type);
  accept(TOsym);
  Expr(&type);
	int hasBy = 0;
	int byVal;
  if (sym == BYsym)
  {
		hasBy = 1;
    confirmSym();
    ConstExpr(&byVal, &type);
  }
  accept(DOsym);
	int exitJmpLoc;
	if(hasBy == 0)
	{
		int loc1 = locCounter;
		insertInstr(for0, 0, 0);
		int loc2 = locCounter;
		exitJmpLoc = StatSeq();
		insertInstr(for1, 0, loc2);
		code[loc1].addr = locCounter;
		if(exitJmpLoc > 1)
			code[exitJmpLoc].addr = locCounter;
	}
	/* FOR statement has a BY clause, more complicated looping structure */
	else if(hasBy == 1)
	{
		insertInstr(sto, 0, -1);
		int address;
		if(symTbl[index].recType == VARREC)
			address = symTbl[index].varRecord.varAddr;
		else if(symTbl[index].recType == PARAMREC)
		{
			if(symTbl[index].paramRecord.variable == 1)
				address = symTbl[index].paramRecord.paramAddr;
			else
				address = symTbl[index].paramRecord.paramAddr;
		}
		insertInstr(sto, 0, address);
		insertInstr(isp, 0, -1);
		int forCheck = locCounter;
		insertInstr(lod, 0, address);
		insertInstr(lod, 0, -1);
		if(byVal > 0)
			insertInstr(le, 0, 0);
		else if(byVal < 0)
			insertInstr(ge, 0, 0);
		else
			compileError(28);
		int condJmp = locCounter;
		insertInstr(jmpc, 0, -1);
		exitJmpLoc = StatSeq();
		
		insertInstr(lod, 0, address);
		insertInstr(lodc, 0, byVal);
		insertInstr(add, 0, 0);
		insertInstr(sto, 0, address);
		
		insertInstr(jmp, 0, forCheck);
		code[condJmp].addr = locCounter;
		if(exitJmpLoc > 1)
			code[exitJmpLoc].addr = locCounter;
	}
  accept(ENDsym);
	
}

/*37. LoopStat -> LOOP StatSeq END */
void LoopStat()
{
  accept(LOOPsym);
	int startLoc = locCounter;
  int exitJmpLoc = StatSeq();
	insertInstr(jmp, 0, startLoc);
  accept(ENDsym);
	if(exitJmpLoc > 1)
		code[exitJmpLoc].addr = locCounter;
}

/*38. CaseStat -> CASE Expr OF Case { '|' Case } [ ELSE StatSeq ] END */
void CaseStat()
{
	/* Note: ideally this would be implemented more efficiently, but the interpreter only contains regular jumps */
	/* At the time of this implemenation the interpreter did not have selc */
  accept(CASEsym);
	int type;
  Expr(&type);
	/* store expr in stack for later comparison */
	insertInstr(sto, 0, 0);
  accept(OFsym);
	int uncondJumps[30];
	int uncondJumpsIndex = 1;
	int uncondJumpsValue;
  uncondJumpsValue = Case();
	uncondJumps[0] = uncondJumpsValue;
  while (sym == PIPE)
  {
    confirmSym();
    uncondJumpsValue = Case();
		uncondJumps[uncondJumpsIndex] = uncondJumpsValue;
		uncondJumpsIndex++;
  }
  if (sym == ELSEsym)
  {
    confirmSym();
    StatSeq();
  }
	
  accept(ENDsym);
	/* all unconditional jumps go here to end of case */
	int i;
	for(i = 0; i < uncondJumpsIndex; i++)
		code[uncondJumps[i]].addr = locCounter;
}

/*39. Case -> [ CaseLabs { , CaseLabs } : StatSeq ] */
/* Ideally implemented with special case instruction, this implemenation is done with jmps based on available interpreter instructions */
int Case()
{
  if (sym == PLUS || sym == MINUS || sym == Integer || sym == Ident || sym == LPAREN || sym == TILDE)
  {
		int patch1, patch2;
    CaseLabs(&patch1, &patch2);
		int patches[20];
		int patchIndex = 0;
		patches[patchIndex++] = patch1;
		if(patch2 >= 0)
		{
			patchIndex--;
			patches[patchIndex++] = patch2;
		}
    while (sym == COMMA)
    {
      confirmSym();
      CaseLabs(&patch1, &patch2);
			patches[patchIndex++] = patch1;
			if(patch2 >= 0)
			{
				patchIndex--;
				patches[patchIndex++] = patch2;
			}
    }
		int i;
		for(i = 0; i < patchIndex - 1; i++)
		{
			code[patches[i]].levelDiff = 1;
			code[patches[i]].addr = locCounter;
		}
    accept(COLON);
    StatSeq();
		int uncondJmpLoc = locCounter;
		insertInstr(jmp, 0, -1);
		code[patch1].addr = locCounter;
		if(patch2 >= 0)
			code[patch2].addr = locCounter;
		
		return uncondJmpLoc;
  }
	return -1;
}

/*40. CaseLabs -> ConstExpr [ .. ConstExpr ] */
void CaseLabs(int *patch1, int *patch2)
{
	int type;
  Expr(&type);
	insertInstr(lod, 0, 0);
  if (sym == DOTDOT)
  {
		insertInstr(le, 0, 0);
		*patch1 = locCounter;
		insertInstr(jmpc, 0, -1);
    confirmSym();
    Expr(&type);
		insertInstr(lod, 0, 0);
		insertInstr(ge, 0, 0);
		*patch2 = locCounter;
		insertInstr(jmpc, 0, -1);
  }
	else
	{
		insertInstr(eq, 0, 0);
		*patch1 = locCounter;
		*patch2 = -1;
		insertInstr(jmpc, 0, -1);
	}
}

/*41. ConstExpr -> Expr */
void ConstExpr(int *value, int *type)
{
	ConstSimplExpr(value, type);
  if (sym == EQUAL || sym == POUND || sym == LT || sym == LE || sym == GT || sym == GE)
  {
		int relop = sym;
    confirmSym();
		int type1;
    ConstSimplExpr(value, &type1);
		checkTypes(*type, type1);
		switch(relop)
		{
			case EQUAL:
				insertInstr(eq, 0, 0);
				break;
			case POUND:
				insertInstr(ne, 0, 0);
				break;
			case LT:
				insertInstr(lt, 0, 0);
				break;
			case LE:
				insertInstr(le, 0, 0);
				break;
			case GT:
				insertInstr(gt, 0, 0);
				break;
			case GE:
				insertInstr(ge, 0, 0);
				break;
		}
		*type = BOOLTYPE;
  }
}

void ConstSimplExpr(int *value, int *type)
{
	if (sym == PLUS || sym == MINUS)
	{
		int addop = sym;
		confirmSym();
		ConstTerm(value, type);
		checkTypes(*type, INTTYPE);
		if(addop == MINUS)
			*value = -1 * *value;
	}
  else
		ConstTerm(value, type);
  while (sym == PLUS || sym == MINUS || sym == ORsym)
  {
		if(sym == ORsym)
			checkTypes(*type, BOOLTYPE);
		else
			checkTypes(*type, INTTYPE);
		
		int addop = sym;
    confirmSym();
		int value1, type1;
    ConstTerm(&value1, &type1);
		switch(addop)
		{
			case PLUS:
				*value = *value + value1;
				break;
			case MINUS:
				*value = *value - value1;
				break;
			case ORsym:
				*value = *value || value1;
				break;
		}
		
		checkTypes(*type, type1);
  }
}
void ConstTerm(int *value, int *type)
{
	ConstFactor(value, type);
  while (sym == MULT || sym == DIVsym || sym == MODsym || sym == AND)
  {
		if(sym == AND)
			checkTypes(*type, BOOLTYPE);
		else
			checkTypes(*type, INTTYPE);
		int mulop = sym;
    confirmSym();
		int value1, type1;
    ConstFactor(&value1, &type1);
		switch(mulop)
		{
			case MULT:
				*value = *value * value1;
				break;
			case DIVsym:
				*value = *value / value1;
				break;
			case MODsym:
				*value = *value % value1;
				break;
			case AND:
				*value = *value && value1;
				break;
		}
		
		checkTypes(*type, type1);
  }
}
void ConstFactor(int *value, int *type)
{
	 if (sym == Ident)
  {
		int index = lookupIdent(varBuffer);
		confirmSym();
		*type = symTbl[index].type;
		
		if(symTbl[index].recType == CONSTREC)
			*value = symTbl[index].constRecord.constVal;
  }
  else if (sym == LPAREN)
  {
    confirmSym();
    ConstExpr(value, type);
    accept(RPAREN);
  }
  else if (sym == TILDE)
  {
    confirmSym();
    ConstFactor(value, type);
		checkTypes(*type, BOOLTYPE);
		*value = -1 * *value;
  }
  else if (sym == Integer)
	{
		*type = INTTYPE;
		*value = intVal;
    confirmSym();
	}
  else
    parseError(4);
}

/*42. Expr -> SimplExpr [ Relation SimplExpr ] */
/* 47. Relation -> = | # | < | <= | > | >=  */
void Expr(int *type)
{
  SimplExpr(type);
  if (sym == EQUAL || sym == POUND || sym == LT || sym == LE || sym == GT || sym == GE)
  {
		int relop = sym;
    confirmSym();
		int type1;
    SimplExpr(&type1);
		checkTypes(*type, type1);
		switch(relop)
		{
			case EQUAL:
				insertInstr(eq, 0, 0);
				break;
			case POUND:
				insertInstr(ne, 0, 0);
				break;
			case LT:
				insertInstr(lt, 0, 0);
				break;
			case LE:
				insertInstr(le, 0, 0);
				break;
			case GT:
				insertInstr(gt, 0, 0);
				break;
			case GE:
				insertInstr(ge, 0, 0);
				break;
		}
		*type = BOOLTYPE;
  }
}

/* 43. SimplExpr -> [ + | - ] Term { AddOp Term } */
/* 46. AddOp -> '+' | - | OR */
void SimplExpr(int *type)
{
  if (sym == PLUS || sym == MINUS)
	{
		int addop = sym;
		confirmSym();
		Term(type);
		checkTypes(*type, INTTYPE);
		if(addop == MINUS)
			insertInstr(neg, 0, 0);
	}
  else
		Term(type);
  while (sym == PLUS || sym == MINUS || sym == ORsym)
  {
		if(sym == ORsym)
			checkTypes(*type, BOOLTYPE);
		else
			checkTypes(*type, INTTYPE);
		
		int addop = sym;
    confirmSym();
		int type1;
    Term(&type1);
		switch(addop)
		{
			case PLUS:
				insertInstr(add, 0, 0);
				break;
			case MINUS:
				insertInstr(sub, 0, 0);
				break;
			case ORsym:
				insertInstr(orl, 0, 0);
				break;
		}
		
		checkTypes(*type, type1);
  }
}

/* 44. Term -> Factor { MulOp Factor } */
/* 48. MulOp -> * | DIV | MOD | & */
void Term(int *type)
{
  Factor(type);
  while (sym == MULT || sym == DIVsym || sym == MODsym || sym == AND)
  {
		if(sym == AND)
			checkTypes(*type, BOOLTYPE);
		else
			checkTypes(*type, INTTYPE);
		int mulop = sym;
    confirmSym();
		int type1;
    Factor(&type1);
		switch(mulop)
		{
			case MULT:
				insertInstr(imul, 0, 0);
				break;
			case DIVsym:
				insertInstr(idiv, 0, 0);
				break;
			case MODsym:
				insertInstr(imod, 0, 0);
				break;
			case AND:
				insertInstr(andl, 0, 0);
				break;
		}
		
		checkTypes(*type, type1);
  }
}

/*45. Factor -> Integer | Designator  [ ActParams ] | '(' Expr ')' | ~ Factor */
/*49. Designator -> Ident { Selector } */
void Factor(int *type)
{
  if (sym == Ident)
  {
		int index = lookupIdent(varBuffer);
		int varAddr = symTbl[index].varRecord.varAddr;
		confirmSym();
		*type = symTbl[index].type;
		int offset = 1;
		int nextIndex = index;
		switch(symTbl[index].recType)
		{
			case CONSTREC:
				insertInstr(lodc, 0, symTbl[index].constRecord.constVal);
				break;
			case VARREC:
				while(sym == PER || sym == LBRAC)
					offset += Selector(&nextIndex, type);

				if(symTbl[index].recType == FIELDREC)
					insertInstr(lod, 0, varAddr + symTbl[index].fieldRecord.fieldAddr);
				else
					insertInstr(lod, currentLevel - symTbl[index].idLevel, symTbl[index].varRecord.varAddr + offset - 1);
				break;
			case PARAMREC:
				while(sym == PER || sym == LBRAC)
					offset += Selector(&nextIndex, type);

				if(symTbl[index].paramRecord.variable == 1)
					insertInstr(lodi, currentLevel - symTbl[index].idLevel, symTbl[index].paramRecord.paramAddr);
				else
					insertInstr(lod, currentLevel - symTbl[index].idLevel, symTbl[index].paramRecord.paramAddr);
				break;
			case PROCREC:
				if(symTbl[index].procRecord.resAddr)
				{
					insertInstr(isp, 0, 1);
					ProcCall(index);
				}
				else
				{
					compileError(14);
					/* need to clear optional paran's */
					if(sym == LPAREN)
					{
						confirmSym();
						while(sym != RPAREN)
							nextSym();
						accept(RPAREN);
					}
				}
				break;
			case STDPROCREC:
				stdProcCall(symTbl[index].stdProcRecord.stdProcNum);
				break;
			default:
				compileError(9);
		}
  }
  else if (sym == LPAREN)
  {
    confirmSym();
    Expr(type);
    accept(RPAREN);
  }
  else if (sym == TILDE)
  {
    confirmSym();
    Factor(type);
		checkTypes(*type, BOOLTYPE);
		insertInstr(notl, 0, 0);
  }
  else if (sym == Integer)
	{
		*type = INTTYPE;
		insertInstr(lodc, 0, intVal);
    confirmSym();
	}
  else
    parseError(4);
}

/*50. Selector -> . Ident | '[' Expr { ',' Expr }  ']' */
int Selector(int *index, int *selType)
{
	int offset = 1;
		
	/* record */
  if (sym == PER)
  {
    confirmSym();
		int i;
		/*for(i = index + 1; i <= typeTbl[symTbl[index].type].recordRecord.lastField; i++) */
		for(i = *index + 1; i <= typeTbl[symTbl[*index].type].size + *index; i++)
		{
			if(strcmp(varBuffer, symTbl[i].idName) == 0) /* field found */
			{
				offset = symTbl[i].fieldRecord.fieldAddr - 1;
				*selType = symTbl[i].type;
				*index = i;
				break;
			}
		}
		if(i == typeTbl[symTbl[*index].type].recordRecord.lastField + 1)
			compileError(25);
    accept(Ident);
  }
	/* array */
  else
  {
		if(typeTbl[*selType].recType == ARRAYREC)
			*selType = typeTbl[*selType].arrayRecord.elementType;
		else
			*selType = typeTbl[symTbl[*index].type].arrayRecord.elementType;
		
    accept(LBRAC);
		int type;
    ConstExpr(&offset, &type);
		int offset2;
    while (sym == COMMA)
    {
      confirmSym();
      ConstExpr(&offset2, &type);
			offset = offset * offset2;
    }
		
		/* array index out of bounds */
		if(offset < 0 || offset >= typeTbl[symTbl[*index].type].size)
			compileError(24);
		
    accept(RBRAC);
		
  }

	return offset;
}

void parseError(int errNum)
{
  PARSER_PRINT("\nERROR ORIGINATING FROM THE PARSER\n");
  switch(errNum)
  {
    case 0:
      break;
    case 1:
      printf("Error: Expecting 'Ident' or 'Integer' but found %s\n", symbols[sym]);
      break;
    case 2:
      printf("Did you mean '.'?\n");
      break;
    case 3:
      printf("Error: Expecting 'Ident', 'ArrayType', 'RecordType', 'EnumType' but found %s\n", symbols[sym]);
      break;
    case 4:
      printf("Error: Expecting 'Ident', 'Integer', '(' or '~' but found %s\n", symbols[sym]);
      break;
    /*case 5: */
      /*printf("Error: Expecting 'Ident', 'Integer', '(' , '~' , '+' , '-' or 'OR' but found %s\n", symbols[sym]); */
      /*break; */
  }
     printf("Line %d: Syntax error at token '%s'\n", curLine, varBuffer );
     printf("> %s\n", lineBuffer);
     printf("> %*c\b%.*s \n", prevLinePtr-varBuffPtr, ' ', varBuffPtr, "^^^^^^^^^^^^^^^^^^^^");
     exit(EXIT_FAILURE);
}

/* inline */ void confirmSym()
{
  #if ERROR_HANDLING
  accept(sym);
  #else
  nextSym();
  #endif
}

/* inline */ int exprCheck()
{
  return (sym == PLUS || sym == MINUS || sym == Integer || sym == TILDE || sym == LPAREN || sym == Ident);
}

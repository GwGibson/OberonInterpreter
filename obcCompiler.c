#include "obcCompiler.h"
#include "obcScanner.h"

int locCounter = 0;
int symTblPoint = 0;
int typeTblPoint = -1;
int currentLevel = 0;
int scopeTbl[SYM_TBL_SIZE];
int numCompErrors = 0;
int numCompWarnings = 0;

/* Used for printing final code */
const char* const opMnemo[] =
{ 
	"hlt",  "ret",  "neg",  "add",  "sub",  "imul", "idiv", "imod", 
	"eq",   "ne",   "lt",   "le",   "gt",   "ge",   "orl",  "andl", 
	"notl", "rdi",  "rdl",  "wri",  "wrl",  "iabs", "isqr",  
	"lod",  "lodc", "loda", "lodi", "sto",  "stoi", "call",
	"isp",  "jmp",  "jmpc",  "for0", "for1", "nop"
};

void compileError(int errNum)
{
  printf("***COMPILER ERROR   ON LINE %3d:   ", curLine);
  switch(errNum)
  {
    case 0:
			printf("Unknown Error.\n");
      break;
    case 1:
      printf("Depth Nesting Limit Reached.\n");
      break;
		case 2:
      printf("Symbol Table Overflow.\n");
      break;
		case 3:
      printf("Type Table Overflow.\n");
      break;
		case 4:
      printf("Max Code Length Reached.\n");
      break;
		case 5:
      printf("Duplicate Identifier Declared Within Scope.\n");
      break;
		case 6:
      printf("Undeclared Identifier.\n");
      break;
		case 7:
			/* error message continued in checkTypes() */
      printf("Type Mismatch. ");
      break;
		case 8:
      printf("Invalid Designator.\n");
      break;
		case 9:
      printf("Invalid Type.");
      break;
		case 10:
      printf("Identifier Expected.\n");
      break;
		case 11:
      printf("Invalid Parameter Type.\n");
      break;
		case 12:
      printf("Invalid Assignment Statement.\n");
      break;
		case 13:
      printf("Read Requires Parameters.\n");
      break;
		case 14:
      printf("Non-returning Procedure Cannot Be On The RHS Of Assignment Statement.\n");
      break;
		case 15:
      printf("Attempting To Modify Constant.\n");
      break;
		case 16:
      printf("Invalid Number Of Parameters For Procedure Call.\n");
      break;
		case 17:
      printf("ABS Must Be Called With Integer Value.\n");
      break;
		case 18:
      printf("ABS Or ODD Must Not Be LHS Of Assignment.\n");
      break;
		case 19:
      printf("ODD Must Be Called With Integer Value.\n");
      break;
		case 20:
      printf("MODULE Name Does Not Match.\n");
      break;
		case 21:
      printf("PROCEDURE Name At End Of Block Does Not Match PROCEDURE Declaration.\n");
      break;
		case 22:
      printf("RETURN In Void PROCEDURE.\n");
      break;
		case 23:
      printf("No RETURN In Non-Void PROCEDURE.\n");
      break;
		case 24:
      printf("Array Index Out Of Bounds.\n");
      break;
		case 25:
      printf("Invalid Record Field.\n");
      break;
		case 26:
      printf("Cannot Select Field Of Non-Record.\n");
      break;
		case 27:
      printf("Cannot Index Non-Array.\n");
      break;
		case 28:
      printf("FOR BY Value Cannot be 0.\n");
      break;
		case 29:
      printf("Cannot Use Literal Value For Pass by Reference Parameter.\n");
      break;
		case 30:
			/* error message continued in checkParamTypes() */
      printf("Invalid Parameter Type For PROCEDURE Call. ");
      break;
  }
	numCompErrors++;
}

void compileWarning(int warnNum)
{
  printf(" * COMPILER WARNING ON LINE %3d:   ", curLine);
  switch(warnNum)
  {
    case 0:
			printf("Unknown Warning.\n");
      break;
    case 1:
      printf("Unnecessary Semi-Colon After Return.\n");
      break;
		case 2:
      printf("Unreachable Code After Return.\n");
      break;
		case 3:
      printf("Unreachable Code After Exit.\n");
      break;
  }
	numCompWarnings++;
}

void printTbls()
{
	/* Printing Symbol Table */
	printf("\n----------------------------------------------------------------------------------------\n");
	printf("| %-5s | %-14s | %-7s | %-7s | %-7s | %-29s |", 
		     "Index", "Name", "Level", "Type", "PrevID", "Address(es)");
	printf("\n----------------------------------------------------------------------------------------\n");
	int i;
	for(i = 0; i <= symTblPoint; i++)
	{
		printf("| %-5d | %-14s | %-7d | %-7d | %-7d |", 
		       i, symTbl[i].idName, symTbl[i].idLevel, symTbl[i].type, symTbl[i].prevID);
		switch(symTbl[i].recType)
		{
			case VARREC:
				printf(" VarAddr:%10d %10s |", symTbl[i].varRecord.varAddr, "");
				break;
			case PROCREC:
				if(symTbl[i].procRecord.resAddr)
					printf(" EntA:%4d, LasP:%3d, ResA:%3d |", symTbl[i].procRecord.entryAddr, symTbl[i].procRecord.lastParam, symTbl[i].procRecord.resAddr);
				else
					printf(" EntryAddr:%4d, LastParam:%3d |", symTbl[i].procRecord.entryAddr, symTbl[i].procRecord.lastParam);
				break;
			case PARAMREC:
				printf(" ParamAddr:%4d,  Variable:%3d |", symTbl[i].paramRecord.paramAddr, symTbl[i].paramRecord.variable);
				break;
			case CONSTREC:
				printf(" ConstValue:%7d %11s|", symTbl[i].constRecord.constVal, "");
				break;
			case STDPROCREC:
				printf(" %-29s |", "Standard proc");
				break;
			case TYPEREC:
				printf(" %-29s |", "Type");
				break;
			case FIELDREC:
				printf(" FieldAddr:%11d %8s|", symTbl[i].fieldRecord.fieldAddr, "");
				break;
			default:
				printf(" %-29s |", "N/A");
				break;
		}
		printf("\n----------------------------------------------------------------------------------------\n");
	}
	
	/* Printing Type Table */
	printf("\n----------------------------------------------------------\n");
	printf("| %-5s | %-10s | %-10s | %-20s |", "Index", "Size", "Type", "Vals");
	printf("\n----------------------------------------------------------\n");

	for(i = 0; i <= typeTblPoint; i++)
	{
		printf("| %-5d | %-10d | %-10s |", i, typeTbl[i].size, typeTbl[i].name);
		switch(typeTbl[i].recType)
		{
			case SCALARREC:
				printf(" %-20s |", "Scalar");
				break;
			case ENUMREC:
				printf(" %5s %7d |", "Last Enum: ", typeTbl[i].enumRecord.lastEnum);
				break;
			case ARRAYREC:
				printf(" %5s %d %4s %2d |", "iType: ", typeTbl[i].arrayRecord.indexType, "eType: ", typeTbl[i].arrayRecord.elementType);
				break;
			case RECREC:
				printf(" %5s %7d |", "Last Field: ", typeTbl[i].recordRecord.lastField);
				break;
		}
		printf("\n----------------------------------------------------------\n");
	}
	
	/* Printing Scope Table */
	printf("\n------------------\n");
	printf("| %-5s | %-6s |", "Scope", "LastID");
	printf("\n------------------\n");
	
	for(i = 0; i <= currentLevel; i++)
	{
		printf("| %-5d | %-6d |", i, scopeTbl[i]);
		printf("\n------------------\n");
	}
}

void initSymTbl()
{
	symTbl[0].recType = -1;
	/* Predefined identifiers */
	insertIdent("ABS", INTTYPE, STDPROCREC);
	symTbl[symTblPoint].stdProcRecord.stdProcNum = 1;
	insertIdent("ODD", BOOLTYPE, STDPROCREC);
	symTbl[symTblPoint].stdProcRecord.stdProcNum = 2;
	insertIdent("READ", NOTYPE, STDPROCREC);
	symTbl[symTblPoint].stdProcRecord.stdProcNum = 3;
	insertIdent("READLN", NOTYPE, STDPROCREC);
	symTbl[symTblPoint].stdProcRecord.stdProcNum = 4;
	insertIdent("WRITE", NOTYPE, STDPROCREC);
	symTbl[symTblPoint].stdProcRecord.stdProcNum = 5;
	insertIdent("WRITELN", NOTYPE, STDPROCREC);
	symTbl[symTblPoint].stdProcRecord.stdProcNum = 6;
	insertIdent("INTEGER", INTTYPE, TYPEREC);
	insertIdent("BOOLEAN", BOOLTYPE, TYPEREC);
	insertIdent("FALSE", BOOLTYPE, CONSTREC);
	insertIdent("TRUE", BOOLTYPE, CONSTREC);
	symTbl[symTblPoint].constRecord.constVal = 1;
}

void initTypeTbl()
{
	/* Scalar types */
	insertType("NoType", 0, SCALARREC);
	insertType("Integer", 1, SCALARREC);
	insertType("Boolean", 1, SCALARREC);
}

void insertIdent(const char idName[], int type, int recordType)
{
	strcpy(symTbl[0].idName, idName);
	int index = scopeTbl[currentLevel];
	
	/* Find ident within current scope */
	while(strcmp(symTbl[index].idName, idName) != 0)
		index = symTbl[index].prevID;
	
	/* Duplicate identifier within current scope */
	if(index != 0)
	{
		compileError(5);
		return;
	}
	
	symTblPoint++;
	if(symTblPoint > SYM_TBL_SIZE)
	{
		symTblPoint--;
		compileError(2);
		return;
	}
	
	/* Insert the new identifier */
	strcpy(symTbl[symTblPoint].idName, idName);
	symTbl[symTblPoint].recType = recordType;
	symTbl[symTblPoint].idLevel = currentLevel;
	symTbl[symTblPoint].type = type;
	
	symTbl[symTblPoint].prevID = scopeTbl[currentLevel];
	scopeTbl[currentLevel] = symTblPoint;
}

void insertType(const char name[], int size, int recordType)
{
	typeTblPoint++;
	
	if(typeTblPoint > TYPE_TBL_SIZE)
	{
		typeTblPoint--;
		compileError(3);
		return;
	}
	
	typeTbl[typeTblPoint].size = size;
	strcpy(typeTbl[typeTblPoint].name, name);
	typeTbl[typeTblPoint].recType = recordType;
}

void enterScope(void)
{
	if(currentLevel < MAX_DEPTH)
	{
		currentLevel++;
		scopeTbl[currentLevel] = 0;
	}
	else
		compileError(1);
		
}

void exitScope(void)
{
	currentLevel--;
	symTblPoint = scopeTbl[currentLevel];
}

void exitRecScope(void)
{
	currentLevel--;
}

void insertInstr(int op, int level, int addr)
{
	if(level >= MAX_DEPTH || addr >= MAX_ADDRESS) 
		return;
	
	if(locCounter > MAX_CODE_RANGE)
	{
		compileError(4);
		return;
	}
	
	code[locCounter].op = op;
	code[locCounter].levelDiff = level;
	code[locCounter].addr = addr;
	
	locCounter++;
}

int lookupIdent(const char idName[])
{
	int levelIndex = currentLevel;
	
	strcpy(symTbl[0].idName, idName);
	
	int index;
	do {
		index = scopeTbl[levelIndex];
		/* check current scope */
		while(strcmp(symTbl[index].idName, idName) != 0)
			index = symTbl[index].prevID;
		levelIndex--;
	}while(index == 0 && levelIndex >= 0);
	
	if(index == 0)
	{
		compileError(6);
		return -1;
	}
	else
		return index;
}

void checkTypes(int index1, int index2)
{
	if(index1 != index2)
	{
		char* type1 = typeTbl[index1].name;
		if(index1 == -1)
			type1 = "UNDEFINED";
		compileError(7);
		printf("Found Type %s And Type %s.\n", type1, typeTbl[index2].name);
	}
}

void checkParamTypes(int index1, int index2)
{
	if(index1 != index2)
	{
		compileError(30);
		printf("Parameter Should be Type %s, Type %s Provided.\n", typeTbl[index1].name, typeTbl[index2].name);
	}
}

void printCode()
{
	struct instr current;
	int index = 0;
	printf("\nCode:\n");
	while(index < locCounter)
	{
		current = code[index];
		printf("%-3d -> %-5s %3d %3d\n", index, opMnemo[current.op], current.levelDiff, current.addr);
		index++;
	}
}

void outputCode(FILE* fileOut)
{
	struct instr current;
	int index = 0;
	while(index < locCounter)
	{
		current = code[index];
		fprintf(fileOut, "%-5d %3d %3d\n", current.op, current.levelDiff, current.addr);
		index++;
	}
	
	printf("\n~~ Output Code Written To code.txt. %d Warning(s) Found. ~~\n\n", numCompWarnings);
}























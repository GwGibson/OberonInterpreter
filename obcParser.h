/* Oberon-S RD Parser */

#ifndef obcParser_h
#define obcParser_h

/* Direct from grammar */
void Module(void);
void Block(int);
void DeclSeq(int*);
void ConstDecls(void);
void ConstDecl(void);
void TypeDecls(void);
void TypeDecl(void);
void Type(int*, int*);
void ArrayType(int*,int*);
void RecordType(int*,int*);
void EnumType(int*, int*);
void VarDecls(int*);
void VarDecl(int*);
void IdentList(int, int*, int*);
void ProcDecl(void);
void FormalPars(int, int*);
void FPSection(void);
void FieldList(int*, int*);
int StatSeq(void);
int Stat(void);
void AssignStat(int);
void ProcCall(int);
void ActParams(int, int*);
int IfStat(void);
void WhileStat(void);
void RepeatStat(void);
void ForStat(void);
void LoopStat(void);
void CaseStat(void);
int Case(void);
void CaseLabs(int*,int*);
void ConstExpr(int*,int*);
void ConstSimplExpr(int*,int*);
void ConstTerm(int*,int*);
void ConstFactor(int*,int*);
void Expr(int*);
void SimplExpr(int*);
void Term(int*);
void Factor(int*);
void Designator(int);
int Selector(int*, int*);

/* Additional parser functions */
/*inline*/ void accept(int);
void finalAccept(int);
/* Proc(void) and Assign(void) require a lookahead mechanism */
void procOrAssign(void);
/* ReadParams and WriteParams need a lookahead mechanism */
void parseError(int);
void parseWarning(int);
/* Attempting to handle errors */
/*inline*/ void confirmSym(void);
/* Checks if the current symbol is one of the many symbols that start an Expr(void) */
/* Return true if it is and false otherwise */
/*inline*/ int exprCheck(void);

/* Additional compiler functions */
void addressParams(int, int*);
void stdProcCall(int);

#endif /* obcParser */


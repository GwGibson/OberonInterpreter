#ifndef obcCompiler_h
#define obcCompiler_h

#include <stdio.h> 
#include <string.h>

#define MAX_DEPTH      10
#define MAX_ADDRESS    1023
#define MAX_CODE_RANGE 1023
#define SYM_TBL_SIZE   256
#define TYPE_TBL_SIZE  128
#define MAX_VAR_LEN    100 

enum opCodes
{ 
	hlt,  ret,  neg,  add,  sub,  imul, idiv, imod, 
	eq,   ne,   lt,   le,   gt,   ge,   orl,  andl, 
	notl, rdi,  rdl,  wri,  wrl,  iabs, isqr,  
	lod,  lodc, loda, lodi, sto,  stoi, call, 
	isp,  jmp,  jmpc, for0, for1, nop
};

extern int locCounter; 
extern int currentLevel;
extern int symTblPoint;
extern int typeTblPoint;
extern int scopeTbl[]; /* Points to last entry for each scope level */
extern int numCompErrors;
extern int numCompWarnings;

/* Instructions */
struct instr 
{
	int op;
	int levelDiff; /* 0 - MAX_DEPTH */
	int addr;      
};
struct instr code[MAX_CODE_RANGE];

/* Symbol table entires */
enum SRecordType { TYPEREC, CONSTREC, FIELDREC, VARREC, PROCREC, PARAMREC, STDPROCREC };
struct identRecord
{
	char idName[MAX_VAR_LEN];
	int prevID;   				
	int idLevel; 					
	int type; 						
	enum SRecordType recType;
	struct /* CONSTREC */
	{
		int constVal;
	} constRecord;
	struct /* FIELDREC */
	{
		int fieldAddr;
	} fieldRecord;
	struct /* VARREC */
	{
		int varAddr; 		
	} varRecord;
	struct /* PROCREC */
	{
		int entryAddr; 	
		int lastParam; 			
		int resAddr; 
	} procRecord;
	struct /* PARAMREC */
	{
		int variable; /* if param is variable */
		int paramAddr;
	} paramRecord;
	struct /* STDPROCREC */
	{
		int stdProcNum;
	} stdProcRecord;
};
struct identRecord symTbl[SYM_TBL_SIZE];

/* Type table */
enum StandardTypes { NOTYPE, INTTYPE, BOOLTYPE };
enum TRecordType { SCALARREC, ENUMREC, ARRAYREC, RECREC };
struct typeRecord
{
	char name[20];
	int size;
	int symTblIndex;
	enum TRecordType recType;
	struct /* ENUMREC */
	{
		int lastEnum; 
	} enumRecord;
	struct /* ARRAYREC */
	{
		int indexType;
		int elementType; 
	} arrayRecord;
	struct /* RECREC */
	{
		int lastField; 
	} recordRecord;
};
struct typeRecord typeTbl[TYPE_TBL_SIZE];

void printTbls(void);
void compileError(int);
void compileWarning(int);
void initSymTbl(void);
void initTypeTbl(void);
void insertIdent(const char[], int, int);
void insertType(const char[], int, int);
void enterScope(void);
void exitScope(void);
void exitRecScope(void);
int lookupIdent(const char[]);
void insertInstr(int, int, int);
void checkTypes(int, int);
void checkParamTypes(int, int);
void printCode();
void outputCode(FILE*);

#endif /* obcCompiler_h */


		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		

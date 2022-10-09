/* Oberon-S Scanner - ANSI VERSION
 *
 * TO ENABLE DETAILED SCANNER OUTPUT USE DIRECTIVE (*s+*) in test code
 * TO DISABLE USE DIRECTIVE (*-s*)
 * As long as the s+ or s- directly follows the first '*' symbol
 * you can have anything following. Will not trigger in nested comments.
 *
 * Will throw an unnecessary error if there are
 * blank lines or open comments at the end of the file.
 * Not worrying about it since the parser takes care of it.
 *
 * Also accepts input directly from stdin, just execute the program
 * and start typing. Prints results from a line behind so you have to enter
 * 2 lines before any feedback. Make sure to enable output using (*s+*) first.
 *
 * The scanner buffers and prints comments when enable_output
 * is set. This is just for testing/marking purposes. Comments
 * are properly ignored when enable_output is not set.
 *
 * Should handle any variation of comments (quadruple nested loops
 * with blank lines in between - you name it)
 *
 */

#ifndef obcScanner_h
#define obcScanner_h

#include <stdio.h>  /* I/O */
#include <stdlib.h> /* EXIT_STATUSES */


/* Preprocessor directives to enable/disable PARSER output & detailed debugging msgs etc. */
/* Scanner output is turned on/off by directives in the target code (*s+*) and (*s-*) */

#define ENABLE_PARSER   1  /* Set to 0 to disable the parser */
#define PARSER_OUTPUT   0  /* Set to 0 to stop detailed parser output (only matters if ENABLE_PARSER) */
#define SYM_TABLE_OUT   0  /* Prints symbol table progression during compilation */
#define PRINT_FNL_COD   1  /* Prints the final intermediate code at the end for user */

#define ERROR_HANDLING  1  /* Set to 0 for NO error handling attempts (they are VERY rudimetary) */
#define DEBUG           0  /* Set to 0 to removed debugging messages */
#define ENABLE_HASH     1  /* Set to 1 to use a hash lookup for reserved words (0 is seq. search lookup) */


/* Parser output setup (for marking purposes) */
#if defined(PARSER_OUTPUT) && PARSER_OUTPUT == 1
#define PARSER_PRINT(fmt, args...) fprintf(stdout, fmt, ##args)
#else
#define PARSER_PRINT(fmt, args...)
#endif

/* For debugging */
#if defined(DEBUG) && DEBUG == 1
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "%s() in %s, line %i: " fmt "\n", \
    __FUNCTION__, __FILE__, __LINE__, ##args)
#else
#define DEBUG_PRINT(fmt, args...)
#endif

#define NUM_SYMS       53  /* currently not used but may be needed later */
#define NUM_RES_WORDS  28
#define MAX_LINE_LEN   255
#define MAX_VAR_LEN    100 /* can be reduced if we dont need comment output */

extern const char* const symbols[];   /* used in parser */

extern char lineBuffer[];             /* main */
extern char varBuffer[];              /* parser */
extern FILE* file;                    /* main */
extern FILE* outFile;                 /* main */
extern unsigned int lineBuffPtr;      /* main */
extern unsigned int prevLinePtr;      /* parser */
extern unsigned int varBuffPtr;       /* parser */
extern unsigned int curLine;          /* parser */
extern int sym;                       /* parser */
extern int endOfLine;                 /* main */
extern short int EOFflag;             /* main */
extern int intVal;                    /* compiler */

/* Ordered by appearance in the grammar. Could re-order to */
/* accommodate hash lookup and get rid of struct array */
enum tokens
{
  MODULEsym, BEGINsym, ENDsym, CONSTsym, TYPEsym, VARsym, PROCEDUREsym,
  ARRAYsym, OFsym, RECORDsym, EXITsym, RETURNsym, IFsym, THENsym, ELSIFsym,
  ELSEsym, WHILEsym, DOsym, REPEATsym, UNTILsym, FORsym, TOsym, BYsym,
  LOOPsym, CASEsym, ORsym, DIVsym, MODsym, UNKNOWN,
  PLUS, MINUS, TILDE, EQUAL, POUND, LT, LE, GT, GE,
  AND, Integer, Ident, MULT, PER, DOTDOT, COMMA,
  COLON, SEMIC, LBRAC, RBRAC, LPAREN, RPAREN, PIPE, ASSGN
};

/* Returns the next symbol from the input file */
void nextSym(void);

/* Called when the end of a line is reached. Fills the buffer with the next line */
/* Throws an error if the incoming line exceeds MAX_LIN_LENGTH */
void newLine(void);

/* Fills the line buffer with the next line in the input file */
int setLineBuffer(void);

/* Tokenizes items that start with letters (Ident and reserved words) */
void getAlphaToken(void);

/* Tokenizes items that start with digits (Integers) */
void getDigitToken(void);

/* Tokenizes all the miscellaneous items */
void getMiscToken(void);

/* Deals with comments, returns -1 if comment is over and 0 otherwise */
int inComment(void);

/* Turns on or off the Scanner output directive */
void checkDirective(void);

/* Helper for inComment - skips characters until a relevant one is found */
void commentSkip(void);

/* Helper for inComment - deals with newlines within a comment */
void commentNewLine(void);

/* Helper for inComment - checks if the comment ends when a '*' is seen */
int commentCheckClose(void);

/* Helper for inComment - checks for nested comments when a '(' is seen */
void commentCheckOpen(void);

/* Determines if the input word is an Ident or a reserved word. */
/* Returns the index of the reserved word or -1 */
#if ENABLE_HASH /* Implementation at the very bottom of obcScanner.c */
struct resWrd{const char* name; unsigned int token;};
struct resWrd *checkResWord(const char*, unsigned int);
#else
int checkResWord(const char*);
#endif

/* Deals with double tokens (<= >= := ..) */
void doubleToken(int, int, char);

void scanError(int);

void scanWarning(int);

/* Sets next to be the nextChar in the line */
/*inline*/ void nextChar(void); /* ANSI doesn't support inline  */

/* Skips whitespace */
/*inline*/ void skipSep(void);

/* Returns true if next is a hex char [0-9]|[A-F] */
/*inline*/ int isHexChar(void);

/* Returns true if next is a letter [a-zA-z] */
/*inline*/ int isAlpha(void);

/* Returns true if next is a digit (0-9) */
/*inline*/ int isDigit(void);

/* Tests if 2 words (strings) are the same */
/* Returns 1 if equal and 0 otherwise */
int cmpWords(const char*, const char*);

/* Can remove if diagnostic output not req'd */
void printLine(void);

/* Converts scanned number into decimal integer */
void intValue(int);

#endif /* obcScanner_h */

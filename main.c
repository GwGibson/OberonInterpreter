#include "obcScanner.h"
#include "obcParser.h"
#include "obcCompiler.h"

int main(int argc, char** argv)
{
  /* Get file from stdin or input arg if there is one*/
  file = (argc < 2) ? stdin : fopen(argv[1], "r");
  if (!file)
  {
    perror(argv[1]);
    exit(EXIT_FAILURE);
  }
	initSymTbl();
	initTypeTbl();

#if ENABLE_PARSER
	printf("\n");
  nextSym();
  Module();
	
	outFile = fopen("code.txt", "w");
	if(numCompErrors == 0)
	{
		#if PRINT_FNL_COD
			printCode();
		#endif
		outputCode(outFile);
	}
	else
		printf("\n*****Code Not Generated. %d Error(s) And %d Warning(s) Found.*****\n\n", numCompErrors, numCompWarnings);
	
	fclose(outFile);
#else /* Just the scanner*/
  while (!EOFflag || (unsigned int)endOfLine+1 != lineBuffPtr)
    nextSym();
#endif
  fclose(file);
  exit(EXIT_SUCCESS);
}

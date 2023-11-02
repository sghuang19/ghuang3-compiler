#include <stdio.h>

extern int yyparse();
extern FILE* yyin;
extern int yydebug;

int parse(const char* filename)
{
	yydebug = 0;
	yyin = fopen(filename, "r");
	if (!yyin)
	{
		fprintf(stderr, "Could not open file %s\n", filename);
		return 1;
	}
	int result = yyparse();
	fclose(yyin);
	return result;
}
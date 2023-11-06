#include <stdio.h>

extern int yyparse();
extern FILE* yyin;
extern int yydebug;

struct decl* parse(FILE* fp)
{
	yydebug = 0;
	int result = yyparse();
	return result;
}
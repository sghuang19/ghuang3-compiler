#include <stdio.h>
#include "parser.h"

extern int yyparse();
extern FILE* yyin;
extern int yydebug;
extern struct decl* root;

struct decl* parse(FILE* fp)
{
	yydebug = 0;
	yyparse();
	return root;
}

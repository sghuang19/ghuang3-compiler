#include <stdio.h>
#include "parser.h"

extern int yyparse();
extern void yyrestart(FILE*);
extern int yydebug;
extern struct decl* root;

struct decl* parse(FILE* fp)
{
	yydebug = 0;
	yyrestart(fp);
	yyparse();
	return root;
}

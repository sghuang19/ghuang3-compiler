#include <stdio.h>
#include <unistd.h>
#include "parser.h"

extern int yyparse();
extern FILE* yyin;
extern int yydebug;
extern struct decl* root;

struct decl* parse(FILE* fp)
{
	yydebug = 0;
	yyin = fp;

	fflush(stdout);
	int original = dup(fileno(stdout));
	freopen("/dev/null", "w", stdout); // Suppress output

	yyparse();

	fflush(stdout);
	dup2(original, fileno(stdout));
	close(original);

	return root;
}

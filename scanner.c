#include <stdio.h>
#include "token.h"

extern FILE* yyin;
extern int yylex();
extern char* yytext;

int scan(FILE* fp)
{
	yyin = fp;
	int token = 1; // Initialize to a non-zero value
	while (token != TOKEN_EOF)
	{
		// Handle the tokens
		if (token == TOKEN_ERROR)
			return 1;
		token = yylex();
	}
	return 0;
}
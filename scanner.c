#include <stdio.h>
#include "token.h"

extern FILE* yyin;
extern int yylex();
extern char* yytext;

int scan(const char* filename)
{
	yyin = fopen(filename, "r");
	if (yyin == NULL)
	{
		printf("Could not open file %s\n", filename);
		return 1;
	}

	int token = 1; // Initialize to a non-zero value
	while (token != TOKEN_EOF)
	{
		// Handle the tokens
		if (token == TOKEN_ERROR) {
			return 1;
		}
		token = yylex();
	}

	fclose(yyin);
	return 0;
}
%{
#include <stdio.h>
#include <string.h>
#include "token.h"
#include "encoder.h"

static int returned_eof = 0;
%}

%option yylineno

%%
    /* Comments */

\/\/[^\n]* {
    printf("INLINE_COMMENT\n");
}

\/\*[^*]*\*(\*|[^*/][^*]*\*)*\/ {
    printf("MULTI_LINE_COMMENT\n");
}

    /* Keywords */

array       { printf("ARRAY\n");    return TOKEN_ARRAY; }
auto        { printf("AUTO\n");     return TOKEN_AUTO; }
boolean     { printf("BOOLEAN\n");  return TOKEN_BOOLEAN; }
char        { printf("CHAR\n");     return TOKEN_CHAR; }
else        { printf("ELSE\n");     return TOKEN_ELSE; }
false       { printf("FALSE\n");    return TOKEN_FALSE; }
float       { printf("FLOAT\n");    return TOKEN_FLOAT; }
for         { printf("FOR\n");      return TOKEN_FOR; }
function    { printf("FUNCTION\n"); return TOKEN_FUNCTION; }
if          { printf("IF\n");       return TOKEN_IF; }
integer     { printf("INTEGER\n");  return TOKEN_INTEGER; }
print       { printf("PRINT\n");    return TOKEN_PRINT; }
return      { printf("RETURN\n");   return TOKEN_RETURN; }
string      { printf("STRING\n");   return TOKEN_STRING; }
true        { printf("TRUE\n");     return TOKEN_TRUE; }
void        { printf("VOID\n");     return TOKEN_VOID; }
while       { printf("WHILE\n");    return TOKEN_WHILE; }

    /* Identifiers and Literals */
    /* Literals have higher precedence to ensure preceding plus and minus
       operators are treated as part of numbers */

[a-zA-Z_][a-zA-Z0-9_]* {
    printf("IDENTIFIER: %s\n", yytext);
    yylval.s = strdup(yytext);
    return TOKEN_ID;
}

[1-9][0-9]*|0 {
    printf("INTEGER_LITERAL: %s\n", yytext);
    yylval.i = atoi(yytext);
    return TOKEN_INT_LITERAL;
}

[0-9]*\.[0-9]+ {
    /* Float with decimal point */
    printf("TOKEN_FLOAT_LITERAL: %s\n", yytext);
    yylval.f = atof(yytext);
    return TOKEN_FLOAT_LITERAL;
}

[0-9]*(\.[0-9]+)?[eE][+-]?[1-9]+[0-9]* {
    /* Float with scientific notation */
    printf("TOKEN_FLOAT_LITERAL: %s\n", yytext);
    yylval.f = atof(yytext);
    return TOKEN_FLOAT_LITERAL;
}

'([[:print:]]|\\([abefnrtv\\'"]|0x[[:xdigit:]]{2}))' {
    // Reuse the string_decode and string_encode functions
    printf("matching: %s\n", yytext);
    char es[strlen(yytext)];
    strcpy(es, yytext);
    // Convert it to a string literal
    es[0] = '"';
    es[strlen(es) - 1] = '"';
    printf("becomes: %s\n", es);
    char s[2];
    if (string_decode((const char*) es, s) == 0) {
        printf("decoded: %s\n", s);
        string_encode((const char*) s, es);
        printf("encoded: %s\n", es);
        es[strlen(es) - 1] = '\0';
        printf("CHAR_LITERAL: %s\n", es + 1);
        yylval.c = s[0];
        return TOKEN_CHAR_LITERAL;
    } else {
        fprintf(stderr, "Scan Error @ %d | Invalid character literal ", yylineno);
        if (*yytext == '\'')
            fprintf(stderr, "%s\n", yytext);
        else
            fprintf(stderr, "'%s'\n", yytext);
        return TOKEN_ERROR;
    }
}

    /* Operators */

\+\+    { printf("INCREMENT\n");        return TOKEN_INCREMENT; }
--      { printf("DECREMENT\n");        return TOKEN_DECREMENT; }
\^      { printf("EXPONENT\n");         return TOKEN_EXPONENT; }
\*      { printf("MULTIPLY\n");         return TOKEN_MULTIPLY; }
\/      { printf("DIVIDE\n");           return TOKEN_DIVIDE; }
\%      { printf("MODULO\n");           return TOKEN_MODULO; }
\+      { printf("ADD\n");              return TOKEN_ADD; }
-       { printf("MINUS\n");            return TOKEN_MINUS; }
\<      { printf("LESS\n");             return TOKEN_LESS; }
\<=     { printf("LESS_OR_EQUAL\n");    return TOKEN_LESS_OR_EQUAL; }
>       { printf("GREATER\n");          return TOKEN_GREATER; }
>=      { printf("GREATER_OR_EQUAL\n"); return TOKEN_GREATER_OR_EQUAL; }
==      { printf("EQUAL\n");            return TOKEN_EQUAL; }
!=      { printf("NOT_EQUAL\n");        return TOKEN_NOT_EQUAL; }
&&      { printf("LOGICAL_AND\n");      return TOKEN_LOGICAL_AND; }
\|\|    { printf("LOGICAL_OR\n");       return TOKEN_LOGICAL_OR; }
!       { printf("LOGICAL_NOT\n");      return TOKEN_LOGICAL_NOT; }
=       { printf("ASSIGN\n");           return TOKEN_ASSIGN; }

    /* Symbols */

"("     { printf("LPAREN\n");       return TOKEN_LPAREN; }
")"     { printf("RPAREN\n");       return TOKEN_RPAREN; }
"["     { printf("LBRACKET\n");     return TOKEN_LBRACKET; }
"]"     { printf("RBRACKET\n");     return TOKEN_RBRACKET; }
"{"     { printf("LBRACE\n");       return TOKEN_LBRACE; }
"}"     { printf("RBRACE\n");       return TOKEN_RBRACE; }
;       { printf("SEMICOLON\n");    return TOKEN_SEMICOLON; }
:       { printf("COLON\n");        return TOKEN_COLON; }
,       { printf("COMMA\n");        return TOKEN_COMMA; }


\"([^"\\]*(\\.[^"\\]*)*)\" {
    char s[MAX_STRING_LEN + 1] = { 0 };
    if (string_decode(yytext, s) == 0) {
        char es[strlen(yytext) + 1];
        string_encode((const char*) s, es);
        printf("STRING_LITERAL: %s\n", es);
        yylval.s = strdup(s);
        return TOKEN_STRING_LITERAL;
    } else {
        fprintf(stderr, "Scan Error @ %d | invalid string literal ", yylineno);
        if (*yytext == '\'')
            fprintf(stderr, "%s\n", yytext);
        else
            fprintf(stderr, "'%s'\n", yytext);
        return TOKEN_ERROR;
    }
}

    /* Others */

[ \t\n\r]+ {
    // Whitespaces ignored
}

<<EOF>> {
    // Returns 0 for subsequent calls
    if (returned_eof) return 0;
    returned_eof = 1;
    return TOKEN_EOF;
}

. {
    fprintf(stderr, "Scan Error @ %d | invalid character '%s'\n", yylineno, yytext);
    return TOKEN_ERROR;
}
%%

int yywrap() {
    return 1;
}

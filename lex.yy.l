%{
#include <stdio.h>
#include <string.h>
#include "token.h"
#include "encoder.h"
%}

%%
    /* Keywords */

array       { printf("ARRAY\n");    return T_ARRAY; }
auto        { printf("AUTO\n");     return T_AUTO; }
boolean     { printf("BOOLEAN\n");  return T_BOOLEAN; }
char        { printf("CHAR\n");     return T_CHAR; }
else        { printf("ELSE\n");     return T_ELSE; }
false       { printf("FALSE\n");    return T_FALSE; }
float       { printf("FLOAT\n");    return T_FLOAT; }
for         { printf("FOR\n");      return T_FOR; }
function    { printf("FUNCTION\n"); return T_FUNCTION; }
if          { printf("IF\n");       return T_IF; }
integer     { printf("INTEGER\n");  return T_INTEGER; }
print       { printf("PRINT\n");    return T_PRINT; }
return      { printf("RETURN\n");   return T_RETURN; }
string      { printf("STRING\n");   return T_STRING; }
true        { printf("TRUE\n");     return T_TRUE; }
print       { printf("TRUE\n");     return T_TRUE; }
void        { printf("VOID\n");     return T_VOID; }
while       { printf("WHILE\n");    return T_WHILE; }

    /* Identifiers and Literals */
    /* Literals have higher precedence to ensure preceding plus and minus
       operators are treated as part of numbers */

[a-zA-Z_][a-zA-Z0-9_]* {
    printf("IDENTIFIER: %s\n", yytext);
    return T_IDENTIFIER;
}

[+-]?([1-9][0-9]*|0) {
    printf("INTEGER_LITERAL: %s\n", yytext);
    return T_INTEGER_LITERAL;
}

[+-]?[0-9]*\.[0-9]+ {
    /* Float without decimal point */
    printf("FLOAT_LITERAL: %s\n", yytext);
    return T_FLOAT_LITERAL;
}

[+-]?[0-9]*(\.[0-9]+)?[eE][+-]?[1-9]+[0-9]* {
    /* Float with scientific notation */
    printf("FLOAT_LITERAL: %s\n", yytext);
    return T_FLOAT_LITERAL;
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
        return T_CHAR_LITERAL;
    } else {
        printf("ERROR: Invalid character literal: %s\n", yytext);
        return T_ERROR;
    }
}
    /* Operators */

\+\+    { printf("INCREMENT\n");        return T_INCREMENT; }
--      { printf("DECREMENT\n");        return T_DECREMENT; }
\^      { printf("EXPONENT\n");         return T_EXPONENT; }
\*      { printf("MULTIPLY\n");         return T_MULTIPLY; }
\/      { printf("DIVIDE\n");           return T_DIVIDE; }
\%      { printf("MODULO\n");           return T_MODULO; }
\+      { printf("ADD\n");              return T_ADD; }
-       { printf("MINUS\n");            return T_MINUS; }
\<      { printf("LESS\n");             return T_LESS; }
\<=     { printf("LESS_OR_EQUAL\n");    return T_LESS_OR_EQUAL; }
>       { printf("GREATER\n");          return T_GREATER; }
>=      { printf("GREATER_OR_EQUAL\n"); return T_GREATER_OR_EQUAL; }
==      { printf("EQUAL\n");            return T_EQUAL; }
!=      { printf("NOT_EQUAL\n");        return T_NOT_EQUAL; }
&&      { printf("LOGICAL_AND\n");      return T_LOGICAL_AND; }
\|\|    { printf("LOGICAL_OR\n");       return T_LOGICAL_OR; }
!       { printf("LOGICAL_NOT\n");      return T_LOGICAL_NOT; }
=       { printf("ASSIGN\n");           return T_ASSIGN; }

    /* Symbols */

"("     { printf("LPAREN\n");       return T_LPAREN; }
")"     { printf("RPAREN\n");       return T_RPAREN; }
"["     { printf("LBRACKET\n");     return T_LBRACKET; }
"]"     { printf("RBRACKET\n");     return T_RBRACKET; }
"{"     { printf("LBRACE\n");       return T_LBRACE; }
"}"     { printf("RBRACE\n");       return T_RBRACE; }
;       { printf("SEMICOLON\n");    return T_SEMICOLON; }
:       { printf("COLON\n");        return T_COLON; }
,       { printf("COMMA\n");        return T_COMMA; }


\"([^"\\]*(\\.[^"\\]*)*)\" {
    char s[MAX_STRING_LEN + 1] = { 0 };
    if (string_decode(yytext, s) == 0) {
        char es[strlen(yytext) + 1];
        string_encode((const char*) s, es);
        printf("STRING_LITERAL: %s\n", es);
        return T_STRING_LITERAL;
    } else {
        printf("ERROR: Invalid string literal: %s\n", yytext);
        return T_ERROR;
    }
}

    /* Others */

[ \t\n\r]+ {
    // Whitespaces ignored
}

<<EOF>> {
    return T_EOF;
}

. {
    printf("ERROR: Invalid character %s\n", yytext);
    return T_ERROR;
}

%%
int yywrap() {
    return 1;
}

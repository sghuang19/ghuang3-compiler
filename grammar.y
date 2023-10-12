%{
#include <stdio.h>
#define YYDEBUG 1

extern int yylex();
extern int yylineno;
extern char *yytext;

void yyerror(const char *s) {
    fprintf(stderr, "Error: %s at line %d near '%s'\n", s, yylineno, yytext);
}
%}

/* Keywords */
%token TOKEN_ARRAY
%token TOKEN_AUTO
%token TOKEN_BOOLEAN
%token TOKEN_CHAR
%token TOKEN_ELSE
%token TOKEN_FALSE
%token TOKEN_FLOAT
%token TOKEN_FOR
%token TOKEN_FUNCTION
%token TOKEN_IF
%token TOKEN_INTEGER
%token TOKEN_PRINT
%token TOKEN_RETURN
%token TOKEN_STRING
%token TOKEN_TRUE
%token TOKEN_VOID
%token TOKEN_WHILE

/* Operators */
%token TOKEN_INCREMENT
%token TOKEN_DECREMENT
%token TOKEN_EXPONENT
%token TOKEN_MULTIPLY
%token TOKEN_DIVIDE
%token TOKEN_MODULO
%token TOKEN_ADD
%token TOKEN_MINUS
%token TOKEN_LESS
%token TOKEN_LESS_OR_EQUAL
%token TOKEN_GREATER
%token TOKEN_GREATER_OR_EQUAL
%token TOKEN_EQUAL
%token TOKEN_NOT_EQUAL
%token TOKEN_LOGICAL_AND
%token TOKEN_LOGICAL_OR
%token TOKEN_LOGICAL_NOT
%token TOKEN_ASSIGN

/* Symbols */
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_LBRACKET
%token TOKEN_RBRACKET
%token TOKEN_LBRACE
%token TOKEN_RBRACE
%token TOKEN_SEMICOLON
%token TOKEN_COLON
%token TOKEN_COMMA

/* Identifiers and Literals */
%token TOKEN_ID
%token TOKEN_INT_LITERAL
%token TOKEN_FLOAT_LITERAL
%token TOKEN_CHAR_LITERAL
%token TOKEN_STRING_LITERAL

/* Error */
%token TOKEN_EOF
%token TOKEN_ERROR

%%
prog : decl_list TOKEN_EOF
     ;

/* Declarations */

decl_list : /* epsilon */
          | decl_list decl
          ;

decl : param TOKEN_SEMICOLON
     | param TOKEN_ASSIGN init
     ;

init : expr TOKEN_SEMICOLON
     | TOKEN_LBRACE opt_stmt_list TOKEN_RBRACE /* function body */
     | TOKEN_LBRACE expr_list TOKEN_RBRACE /* array */
     ;

/* Statements */

opt_stmt_list : /* epsilon */
              | stmt_list
              ;

stmt_list : stmt
          | stmt_list stmt
          ;

stmt : TOKEN_LBRACE stmt_list TOKEN_RBRACE
     | for_loop
     | if_cond
     | print_stmt
     | return_stmt
     | decl
     | expr TOKEN_SEMICOLON
     ;

for_loop : TOKEN_FOR TOKEN_LPAREN
           opt_expr TOKEN_SEMICOLON opt_expr TOKEN_SEMICOLON opt_expr
           TOKEN_RPAREN stmt
         ;

if_cond : TOKEN_IF TOKEN_LPAREN opt_expr TOKEN_RPAREN else_stmt
        ;

else_stmt : TOKEN_ELSE stmt
          | /* epsilon */
          ;

print_stmt : TOKEN_PRINT opt_expr_list TOKEN_SEMICOLON
      ;

return_stmt : TOKEN_RETURN opt_expr TOKEN_SEMICOLON
       ;

/* Expressions */

opt_expr_list : /* epsilon */
              | expr_list
              ;

expr_list : expr
          | expr_list TOKEN_COMMA expr
          ;

opt_expr : /* epsilon */
         | expr
         ;

expr : expr1
     ;

expr1 : lval TOKEN_ASSIGN expr1
      | expr2
      ;

lval : TOKEN_ID
     | TOKEN_ID index
     ;

expr2 : expr2 TOKEN_LOGICAL_OR expr3
      | expr3
      ;

expr3 : expr3 TOKEN_LOGICAL_AND expr4
      | expr4
      ;

expr4 : expr4 TOKEN_EQUAL            expr5
      | expr4 TOKEN_NOT_EQUAL        expr5
      | expr4 TOKEN_LESS             expr5
      | expr4 TOKEN_LESS_OR_EQUAL    expr5
      | expr4 TOKEN_GREATER          expr5
      | expr4 TOKEN_GREATER_OR_EQUAL expr5
      | expr5
      ;

expr5 : expr5 TOKEN_ADD   expr6
      | expr5 TOKEN_MINUS expr6
      | expr6
      ;

expr6 : expr6 TOKEN_MULTIPLY expr7
      | expr6 TOKEN_DIVIDE   expr7
      | expr6 TOKEN_MODULO   expr7
      | expr7
      ;

expr7 : expr7 TOKEN_EXPONENT expr8
      | expr8
      ;

expr8 : TOKEN_MINUS       expr8
      | TOKEN_LOGICAL_NOT expr8
      | expr9
      ;

expr9 : expr9 TOKEN_INCREMENT
      | expr9 TOKEN_DECREMENT
      | group
      ;

group : TOKEN_LPAREN expr TOKEN_RPAREN
      | TOKEN_ID TOKEN_LPAREN opt_expr_list TOKEN_RPAREN
      | TOKEN_ID index
      | factor
      ;

index : TOKEN_LBRACKET expr TOKEN_RBRACKET
      ;

factor : TOKEN_ID
       | TOKEN_INT_LITERAL
       | TOKEN_FLOAT_LITERAL
       | TOKEN_CHAR_LITERAL
       | TOKEN_STRING_LITERAL
       | TOKEN_TRUE
       | TOKEN_FALSE
       ;

/* Types */

type : TOKEN_INTEGER
     | TOKEN_FLOAT
     | TOKEN_BOOLEAN
     | TOKEN_CHAR
     | TOKEN_STRING
     | TOKEN_VOID
     | TOKEN_ARRAY index type
     | type_func
     ;

type_func : TOKEN_FUNCTION type TOKEN_LPAREN opt_param_list TOKEN_RPAREN
          ;

opt_param_list : /* epsilon */
               | param_list
               ;

param_list : param
           | param_list TOKEN_COMMA param
           ;

param : TOKEN_ID TOKEN_COLON type
      ;

%%

// int yywrap() { return 0; }
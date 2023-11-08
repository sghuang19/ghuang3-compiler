%{
#include <stdio.h>
#include "stmt.h"
#define YYDEBUG 1

extern int yylex();
extern int yylineno;
extern char *yytext;

void yyerror(const char *s) {
    fprintf(stderr, "Parse Error @ %d | %s\n", yylineno, s);
}

struct decl *root = NULL;
%}

%error-verbose

/* Keywords */
%token TOKEN_ARRAY
%token TOKEN_AUTO
%token TOKEN_BOOLEAN
%token TOKEN_CHAR
%token TOKEN_ELSE
%token TOKEN_FLOAT
%token TOKEN_FOR
%token TOKEN_FUNCTION
%token TOKEN_IF
%token TOKEN_INTEGER
%token TOKEN_PRINT
%token TOKEN_RETURN
%token TOKEN_STRING
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
%token <s> TOKEN_ID
%token <i> TOKEN_INT_LITERAL
%token <f> TOKEN_FLOAT_LITERAL
%token <c> TOKEN_CHAR_LITERAL
%token <s> TOKEN_STRING_LITERAL
%token <i> TOKEN_TRUE
%token <i> TOKEN_FALSE

/* Error */
%token TOKEN_EOF
%token TOKEN_ERROR

/* Types expected in AST */
%union {
    struct decl *decl;
    struct stmt *stmt;
    struct expr *expr;
    struct param_list *param_list;
    struct type *type;

    char c;
    int i;
    float f;
    char* s;
};

%type <decl> prog decl_list decl decl_init
%type <expr> opt_expr_list expr_list opt_expr expr
             expr1 expr2 expr3 expr4 expr5 expr6 expr7 expr8 expr9
             group lval factor index
%type <stmt> opt_stmt_list stmt_list stmt closed_stmt open_stmt
             if_cond if_stmt_closed if_stmt_open
             for_header for_stmt_open for_stmt_closed
             simple_stmt print_stmt return_stmt block_stmt
%type <type> type_simple type_array type_array_sized type_func
%type <param_list> opt_param_list param_list param

%%
prog : decl_list TOKEN_EOF { $$ = $1; root = $$; YYACCEPT; }
     ;

/* Declarations */

decl_list : /* epsilon */  { $$ = NULL; }
          | decl decl_list { $$ = $1; $$->next = $2; }
          ;

decl : TOKEN_ID TOKEN_COLON type_simple TOKEN_SEMICOLON
       { $$ = decl_create($1, $3, NULL, NULL, NULL); }
     | TOKEN_ID TOKEN_COLON type_array_sized TOKEN_SEMICOLON
       { $$ = decl_create($1, $3, NULL, NULL, NULL); }
     | TOKEN_ID TOKEN_COLON type_func TOKEN_SEMICOLON
       { $$ = decl_create($1, $3, NULL, NULL, NULL); }
     | decl_init
     ;

decl_init : TOKEN_ID TOKEN_COLON type_simple TOKEN_ASSIGN expr TOKEN_SEMICOLON
            { $$ = decl_create($1, $3, $5, NULL, NULL); }
          | TOKEN_ID TOKEN_COLON type_array_sized TOKEN_ASSIGN
            TOKEN_LBRACE opt_expr_list TOKEN_RBRACE TOKEN_SEMICOLON
            { $$ = decl_create($1, $3, $6, NULL, NULL); }
          | TOKEN_ID TOKEN_COLON type_func TOKEN_ASSIGN
            TOKEN_LBRACE opt_stmt_list TOKEN_RBRACE
            { $$ = decl_create($1, $3, NULL, $6, NULL); }
          ;

/* Statements */

opt_stmt_list : /* epsilon */ { $$ = NULL; }
              | stmt_list
              ;

stmt_list : stmt
          | stmt stmt_list { $$ = $1; $$->next = $2; }
          ;

stmt : open_stmt
     | closed_stmt
     ;

closed_stmt : if_stmt_closed
            | for_stmt_closed
            | simple_stmt
            ;

open_stmt : if_stmt_open
          | for_stmt_open
          ;

if_cond : TOKEN_IF TOKEN_LPAREN opt_expr TOKEN_RPAREN
          { $$ = stmt_create_empty(STMT_IF_ELSE); $$->expr = $3; }
        ;

if_stmt_closed : if_cond closed_stmt TOKEN_ELSE closed_stmt
                 { $$ = $1; $$->body = $2; $$->else_body = $4; }
               ;

if_stmt_open : if_cond stmt
               { $$ = $1; $$->body = $2; }
             | if_cond closed_stmt TOKEN_ELSE if_stmt_open
               { $$ = $1; $$->body = $2; $$->else_body = $4;}
             ;

for_header : TOKEN_FOR TOKEN_LPAREN
             opt_expr TOKEN_SEMICOLON opt_expr TOKEN_SEMICOLON opt_expr
             TOKEN_RPAREN
             { $$ = stmt_create(STMT_FOR, NULL, $3, $5, $7, NULL, NULL, NULL); }

for_stmt_open : for_header open_stmt { $$ = $1; $$->body = $2; }
              ;

for_stmt_closed : for_header closed_stmt { $$ = $1; $$->body = $2; }
                ;

/* Simple statements are not recursive */
simple_stmt : print_stmt
            | return_stmt
            | block_stmt
            | decl                 { $$ = stmt_create_empty(STMT_DECL), $$->decl = $1; }
            | expr TOKEN_SEMICOLON { $$ = stmt_create_empty(STMT_EXPR), $$->expr = $1; }
            ;

print_stmt : TOKEN_PRINT opt_expr_list TOKEN_SEMICOLON
             { $$ = stmt_create_empty(STMT_PRINT); $$->expr = $2; }
           ;

return_stmt : TOKEN_RETURN opt_expr TOKEN_SEMICOLON
              { $$ = stmt_create_empty(STMT_RETURN); $$->expr = $2; }
            ;

block_stmt : TOKEN_LBRACE stmt_list TOKEN_RBRACE
             { $$ = stmt_create_empty(STMT_BLOCK); $$->body = $2; }
           ;

/* Expressions */

opt_expr_list : /* epsilon */ { $$ = NULL; }
              | expr_list
              ;

expr_list : expr
            { $$ = expr_create(EXPR_LIST, $1, NULL); }
          | expr TOKEN_COMMA expr_list
            { $$ = expr_create(EXPR_LIST, $1, $3); }
          ;

opt_expr : /* epsilon */ { $$ = NULL; }
         | expr
         ;

expr : expr1
     ;

expr1 : lval TOKEN_ASSIGN expr1 { expr_create(EXPR_ASSIGN, $1, $3); }
      | expr2
      ;

lval : TOKEN_ID       { $$ = expr_create_name($1); }
     | TOKEN_ID index { $$ = expr_create(EXPR_INDEX, expr_create_name($1), $2); }
     ;

expr2 : expr2 TOKEN_LOGICAL_OR expr3 { $$ = expr_create(EXPR_OR, $1, $3); }
      | expr3
      ;

expr3 : expr3 TOKEN_LOGICAL_AND expr4 { $$ = expr_create(EXPR_AND, $1, $3); }
      | expr4
      ;

expr4 : expr4 TOKEN_EQUAL            expr5 { $$ = expr_create(EXPR_EQ,  $1, $3); }
      | expr4 TOKEN_NOT_EQUAL        expr5 { $$ = expr_create(EXPR_NEQ, $1, $3); }
      | expr4 TOKEN_LESS             expr5 { $$ = expr_create(EXPR_LT,  $1, $3); }
      | expr4 TOKEN_LESS_OR_EQUAL    expr5 { $$ = expr_create(EXPR_LEQ, $1, $3); }
      | expr4 TOKEN_GREATER          expr5 { $$ = expr_create(EXPR_GT,  $1, $3); }
      | expr4 TOKEN_GREATER_OR_EQUAL expr5 { $$ = expr_create(EXPR_GEQ, $1, $3); }
      | expr5
      ;

expr5 : expr5 TOKEN_ADD   expr6 { $$ = expr_create(EXPR_ADD, $1, $3); }
      | expr5 TOKEN_MINUS expr6 { $$ = expr_create(EXPR_SUB, $1, $3); }
      | expr6
      ;

expr6 : expr6 TOKEN_MULTIPLY expr7 { $$ = expr_create(EXPR_MUL, $1, $3); }
      | expr6 TOKEN_DIVIDE   expr7 { $$ = expr_create(EXPR_DIV, $1, $3); }
      | expr6 TOKEN_MODULO   expr7 { $$ = expr_create(EXPR_MOD, $1, $3); }
      | expr7
      ;

expr7 : expr7 TOKEN_EXPONENT expr8 { $$ = expr_create(EXPR_EXP, $1, $3); }
      | expr8
      ;

expr8 : TOKEN_MINUS       expr8 { $$ = expr_create_unary(EXPR_NEG, $2); }
      | TOKEN_LOGICAL_NOT expr8 { $$ = expr_create_unary(EXPR_NOT, $2); }
      | expr9
      ;

expr9 : expr9 TOKEN_INCREMENT { $$ = expr_create_unary(EXPR_INCREMENT, $1); }
      | expr9 TOKEN_DECREMENT { $$ = expr_create_unary(EXPR_DECREMENT, $1); }
      | group
      ;

group : TOKEN_LPAREN expr TOKEN_RPAREN { $$ = $2; }
      | TOKEN_ID TOKEN_LPAREN opt_expr_list TOKEN_RPAREN
        { $$ = expr_create(EXPR_CALL, expr_create_name($1), $3); }
      | TOKEN_ID index
        { $$ = expr_create(EXPR_INDEX, expr_create_name($1), $2); }
      | factor
      ;

index : TOKEN_LBRACKET expr TOKEN_RBRACKET { $$ = $2; }
      ;

factor : TOKEN_ID             { $$ = expr_create_name($1); }
       | TOKEN_INT_LITERAL    { $$ = expr_create_integer_literal($1); }
       | TOKEN_FLOAT_LITERAL  { $$ = expr_create_float_literal($1); }
       | TOKEN_CHAR_LITERAL   { $$ = expr_create_char_literal($1); }
       | TOKEN_STRING_LITERAL { $$ = expr_create_string_literal($1); }
       | TOKEN_TRUE           { $$ = expr_create_boolean_literal(1); }
       | TOKEN_FALSE          { $$ = expr_create_boolean_literal(0); }
       ;

/* Types */

type_simple : TOKEN_INTEGER { $$ = type_create(TYPE_INTEGER); }
            | TOKEN_FLOAT   { $$ = type_create(TYPE_FLOAT); }
            | TOKEN_BOOLEAN { $$ = type_create(TYPE_BOOLEAN); }
            | TOKEN_CHAR    { $$ = type_create(TYPE_CHAR); }
            | TOKEN_STRING  { $$ = type_create(TYPE_STRING); }
            | TOKEN_VOID    { $$ = type_create(TYPE_VOID); }
            ;

type_array : TOKEN_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET type_simple
             { $$ = type_create_array($4, NULL); }
           | TOKEN_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET type_array
             { $$ = type_create_array($4, NULL); }
           ;

type_array_sized : TOKEN_ARRAY index type_simple
                   { $$ = type_create_array($3, $2); }
                 | TOKEN_ARRAY index type_array_sized
                   { $$ = type_create_array($3, $2); }
                 ;

type_func : TOKEN_FUNCTION type_simple TOKEN_LPAREN opt_param_list TOKEN_RPAREN
            { $$ = type_create_func($2, $4); }
          | TOKEN_FUNCTION type_array_sized TOKEN_LPAREN opt_param_list TOKEN_RPAREN
            { $$ = type_create_func($2, $4); }
          ;

opt_param_list : /* epsilon */ { $$ = NULL; }
               | param_list
               ;

param_list : param
           | param_list TOKEN_COMMA param { $$ = $1; $1->next = $3; }
           ;

param : TOKEN_ID TOKEN_COLON type_simple      { $$ = param_list_create($1, $3, NULL); }
      | TOKEN_ID TOKEN_COLON type_array       { $$ = param_list_create($1, $3, NULL); }
      | TOKEN_ID TOKEN_COLON type_array_sized { $$ = param_list_create($1, $3, NULL); }
      ;

%%

// int yywrap() { return 0; }
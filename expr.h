#ifndef EXPR_H
#define EXPR_H

#include "symbol.h"

typedef enum
{
	/* Arithmetic operator nodes */
	EXPR_ADD,
	EXPR_SUB,
	EXPR_MUL,
	EXPR_DIV,
	EXPR_MOD,
	EXPR_EXP,

	/* Comparison and logic operator nodes */
	EXPR_GT,
	EXPR_GEQ,
	EXPR_LT,
	EXPR_LEQ,
	EXPR_EQ,
	EXPR_NEQ,
	EXPR_AND,
	EXPR_OR,

	/* Other binary operator nodes */
	EXPR_CALL,
	EXPR_INDEX,
	EXPR_ASSIGN,

	/* Unary operator nodes */
	EXPR_NEG,
	EXPR_NOT,
	EXPR_INCREMENT,
	EXPR_DECREMENT,

	/* Leaf nodes */
	EXPR_INTEGER_LITERAL,
	EXPR_FLOAT_LITERAL,
	EXPR_BOOLEAN_LITERAL,
	EXPR_CHAR_LITERAL,
	EXPR_STRING_LITERAL,
	EXPR_NAME,

	/* Special nodes */
	EXPR_LIST
} expr_t;

struct expr
{
	/* Used by all kinds of expressions */
	expr_t kind;
	struct expr* left;
	struct expr* right;

	/* Used by various leaf expressions */
	struct symbol* symbol;
	int integer_literal; // Used by INTEGER and BOOLEAN
	float float_literal;
	char char_literal;
	const char* string_literal; // Used by STRING and NAME
};

struct expr* expr_create(expr_t kind, struct expr* left, struct expr* right);

/* Creating leaf nodes */
struct expr* expr_create_name(const char* n);
struct expr* expr_create_integer_literal(int c);
struct expr* expr_create_boolean_literal(int c);
struct expr* expr_create_char_literal(char c);
struct expr* expr_create_string_literal(const char* str);

struct expr* expr_create_unary(expr_t kind, struct expr* operand);

void expr_print(const struct expr* e);

#endif

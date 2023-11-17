#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "encoder.h"
#include "expr.h"
#include "param_list.h"
#include "type.h"
#include "scope.h"
#include "symbol.h"

/* Creating binary nodes by default */

struct expr* expr_create(expr_t kind, struct expr* left, struct expr* right)
{
	struct expr* e = malloc(sizeof(*e));
	e->kind = kind;
	e->left = left;
	e->right = right;
	return e;
}

/* Creating unary operation nodes */

struct expr* expr_create_unary(expr_t kind, struct expr* operand)
{
	return expr_create(kind, operand, NULL);
}

/* Creating leaf nodes */

struct expr* expr_create_leaf(expr_t kind)
{
	return expr_create(kind, NULL, NULL);
}

struct expr* expr_create_name(const char* n)
{
	struct expr* e = expr_create_leaf(EXPR_NAME);
	e->name = n;
	return e;
}

struct expr* expr_create_integer_literal(int i)
{
	struct expr* e = expr_create_leaf(EXPR_INTEGER_LITERAL);
	e->integer_literal = i;
	return e;
}

struct expr* expr_create_float_literal(float f)
{
	struct expr* e = expr_create_leaf(EXPR_FLOAT_LITERAL);
	e->float_literal = f;
	return e;
}

struct expr* expr_create_boolean_literal(int b)
{
	struct expr* e = expr_create_leaf(EXPR_BOOLEAN_LITERAL);
	e->integer_literal = b; // Reuse the integer_literal field
	return e;
}

struct expr* expr_create_char_literal(char c)
{
	struct expr* e = expr_create_leaf(EXPR_CHAR_LITERAL);
	e->char_literal = c;
	return e;
}

struct expr* expr_create_string_literal(const char* str)
{
	struct expr* e = expr_create_leaf(EXPR_STRING_LITERAL);
	e->string_literal = str;
	return e;
}

/* Printing the expression nodes */

/**
 * @param expr_t k the kind of the operator
 * @return a string of the operator
 */
char* get_operator(expr_t k)
{
	char* op = calloc(3, sizeof(char));
	switch (k)
	{
		/* Arithmetic operators */
	case EXPR_ADD:
		op[0] = '+';
		break;
	case EXPR_SUB:
		op[0] = '-';
		break;
	case EXPR_MUL:
		op[0] = '*';
		break;
	case EXPR_DIV:
		op[0] = '/';
		break;
	case EXPR_MOD:
		op[0] = '%';
		break;
	case EXPR_EXP:
		op[0] = '^';
		break;

		/* Comparison and logical ops */
	case EXPR_EQ:
		op[0] = op[1] = '=';
		break;
	case EXPR_NEQ:
		op[0] = '!';
		op[1] = '=';
		break;
	case EXPR_LT:
		op[0] = '<';
		break;
	case EXPR_LEQ:
		op[0] = '<';
		op[1] = '=';
		break;
	case EXPR_GT:
		op[0] = '>';
		break;
	case EXPR_GEQ:
		op[0] = '>';
		op[1] = '=';
		break;
	case EXPR_AND:
		op[0] = op[1] = '&';
		break;
	case EXPR_OR:
		op[0] = op[1] = '|';
		break;

		/* Other binary ops */
	case EXPR_CALL:
		op[0] = '(';
		op[1] = ')';
		break;
	case EXPR_INDEX:
		op[0] = '[';
		op[1] = ']';
		break;
	case EXPR_ASSIGN:
		op[0] = '=';
		break;

	default:
		free(op);
		return NULL;
	}
	return op;
}

int get_precedence(const struct expr* e)
{
	switch (e->kind)
	{
	case EXPR_ASSIGN:
		return 1;
	case EXPR_OR:
		return 2;
	case EXPR_AND:
		return 3;
	case EXPR_LT:
	case EXPR_LEQ:
	case EXPR_GT:
	case EXPR_GEQ:
	case EXPR_EQ:
	case EXPR_NEQ:
		return 4;
	case EXPR_ADD:
	case EXPR_SUB:
		return 5;
	case EXPR_MUL:
	case EXPR_DIV:
	case EXPR_MOD:
		return 6;
	case EXPR_EXP:
		return 7;
	case EXPR_NEG:
	case EXPR_NOT:
		return 8;
	case EXPR_INCREMENT:
	case EXPR_DECREMENT:
		return 9;
	default:
		return 10;
	}
}

void expr_print_binary(const struct expr* e)
{
	/* Non-trivial cases handled by expr_print() */
	if (get_precedence(e) > get_precedence(e->left))
	{
		printf("(");
		expr_print(e->left);
		printf(")");
	}
	else
		expr_print(e->left);

	char* op = get_operator(e->kind);
	printf(" %s ", op);
	free(op);

	if (get_precedence(e) > get_precedence(e->right))
	{
		printf("(");
		expr_print(e->right);
		printf(")");
	}
	else
		expr_print(e->right);
}

void expr_print_leaf(const struct expr* e)
{
	char* es;
	switch (e->kind)
	{
	case EXPR_NAME:
		printf("%s", e->name);
		break;
	case EXPR_INTEGER_LITERAL:
		printf("%d", e->integer_literal);
		break;
	case EXPR_FLOAT_LITERAL:
		printf("%f", e->float_literal);
		break;
	case EXPR_BOOLEAN_LITERAL:
		printf("%s", e->integer_literal ? "true" : "false");
		break;
	case EXPR_CHAR_LITERAL:
		printf("'%c'", e->char_literal);
		break;
	case EXPR_STRING_LITERAL:
		es = malloc(MAX_STRING_LEN * sizeof(char) + 1);
		string_encode(e->string_literal, es);
		printf("%s", es);
		break;

	default:
		/* Non-literals handled in expr_print() */
		break;
	}
}

void expr_print_list(const struct expr* e)
{
	expr_print(e->left);
	if (e->right != NULL)
	{
		printf(", ");
		expr_print_list(e->right);
	}
}

void expr_print(const struct expr* e)
{
	switch (e->kind)
	{
		/* Leaf nodes */
	case EXPR_NAME:
	case EXPR_INTEGER_LITERAL:
	case EXPR_FLOAT_LITERAL:
	case EXPR_BOOLEAN_LITERAL:
	case EXPR_CHAR_LITERAL:
	case EXPR_STRING_LITERAL:
		expr_print_leaf(e);
		break;

		/* Other non-trivial operators */
	case EXPR_NEG:
		printf("-");
		expr_print(e->left);
		break;
	case EXPR_NOT:
		printf("!");
		expr_print(e->left);
		break;
	case EXPR_INCREMENT:
		expr_print(e->left);
		printf("++");
		break;
	case EXPR_DECREMENT:
		expr_print(e->left);
		printf("--");
		break;
	case EXPR_CALL:
		expr_print(e->left);
		printf("(");
		expr_print_list(e->right);
		printf(")");
		break;
	case EXPR_INDEX:
		expr_print(e->left);
		printf("[");
		expr_print(e->right);
		printf("]");
		break;

		/* Other nodes */
	case EXPR_LIST:
		expr_print_list(e);
		break;

	default:
		/* Trivial binary operators */
		expr_print_binary(e);
		break;
	}
}

/* Resolving the expression nodes */

void expr_resolve(struct expr* e)
{
	if (!e) return;
	if (e->kind == EXPR_NAME)
	{
		e->symbol = scope_lookup(e->name);
		if (e->symbol)
			resolve_msg(e->symbol);
		else
		{
			fprintf(stderr, "Resolve Error | Undefined symbol '%s'\n", e->name);
			res_errors++;
		}
	}
	else
	{
		expr_resolve(e->left);
		expr_resolve(e->right);
	}
}

/* Typechecking the expressions */

/** @param rt Pass NULL to print a unary type error msg */
void type_error_msg(const struct expr* e, const struct type* lt, const struct type* rt)
{
	// operator "<op>" cannot be applied on <type> (<expr>) and <type> (<expr>)
	char* op = get_operator(e->kind);
	printf("Type Error | operator '%s' cannot be applied on ", op);
	free(op);
	type_print(lt);
	printf(" (");
	expr_print(e->left);
	printf(")");
	if (rt)
	{
		printf(" and ");
		type_print(rt);
		printf(" (");
		expr_print(e->right);
		printf(")");
	}
	printf("\n");
	type_errors++;
}

void arg_error_msg(const struct expr* e)
{
	printf("Type Error | function '%s' ", e->left->name);
	if (!e->left->symbol->type->params)
		printf("does not expect arguments but is given\n");
	else if (!e->right)
		printf("expects arguments but none is given\n");
	else
	{
		printf("called with incorrect arguments (");
		expr_print_list(e->right);
		printf(")\n");
	}
	type_errors++;
}

struct type* expr_typecheck(const struct expr* e)
{
	if (!e) return NULL;
	struct type* lt = expr_typecheck(e->left);
	struct type* rt = expr_typecheck(e->right);

	switch (e->kind)
	{
		/* Arithmetic operators */

	case EXPR_ADD:
	case EXPR_SUB:
	case EXPR_MUL:
	case EXPR_MOD:
	case EXPR_DIV:
	case EXPR_EXP:
		if (lt->kind != rt->kind ||
			lt->kind != TYPE_INTEGER && lt->kind != TYPE_FLOAT)
			type_error_msg(e, lt, rt);
		return lt;

		/* Comparison operators */

	case EXPR_GT:
	case EXPR_GEQ:
	case EXPR_LT:
	case EXPR_LEQ:
	case EXPR_EQ:
	case EXPR_NEQ:
		if (lt->kind != rt->kind ||
			lt->kind != TYPE_INTEGER && lt->kind != TYPE_FLOAT &&
			lt->kind != TYPE_CHAR && lt->kind != TYPE_BOOLEAN)
			type_error_msg(e, lt, rt);
		return type_create(TYPE_BOOLEAN);

		/* Logical operators */

	case EXPR_AND:
	case EXPR_OR:
		if (lt->kind != TYPE_BOOLEAN || rt->kind != TYPE_BOOLEAN)
			type_error_msg(e, lt, rt);
		return type_create(TYPE_BOOLEAN);

		/* Other binary operator nodes */

	case EXPR_CALL:
		if (lt->kind != TYPE_FUNCTION)
		{
			type_error_msg(e, lt, rt);
			return rt ? rt : lt; // Assume same type as argument, otherwise as left
		}
		if (!param_typecheck(lt->params, e->right))
			arg_error_msg(e);
		return e->left->symbol->type->subtype;
	case EXPR_INDEX:
		if (lt->kind != TYPE_ARRAY || rt->kind != TYPE_INTEGER)
			type_error_msg(e, lt, rt);
		return lt->subtype ? lt->subtype : lt;
	case EXPR_ASSIGN:
		if (lt->kind != rt->kind)
			type_error_msg(e, lt, rt);
		return lt;

		/* Unary operator nodes */

	case EXPR_NEG:
	case EXPR_INCREMENT:
	case EXPR_DECREMENT:
		if (lt->kind != TYPE_INTEGER && lt->kind != TYPE_FLOAT)
			type_error_msg(e, lt, NULL);
		return lt;
	case EXPR_NOT:
		if (lt->kind != TYPE_BOOLEAN)
			type_error_msg(e, lt, NULL);
		return type_create(TYPE_BOOLEAN);

		/* Leaf nodes */

	case EXPR_NAME:
		return e->symbol->type;
	case EXPR_INTEGER_LITERAL:
		return type_create(TYPE_INTEGER);
	case EXPR_FLOAT_LITERAL:
		return type_create(TYPE_FLOAT);
	case EXPR_BOOLEAN_LITERAL:
		return type_create(TYPE_BOOLEAN);
	case EXPR_CHAR_LITERAL:
		return type_create(TYPE_CHAR);
	case EXPR_STRING_LITERAL:
		return type_create(TYPE_STRING);

		/* Special nodes */
	case EXPR_LIST:
		// Arguments are checked in EXPR_CALL
		return lt; // Type of list is that of the 1st element
	}
	return NULL;
}

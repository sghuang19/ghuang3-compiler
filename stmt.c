#include <stdlib.h>
#include <stdio.h>

#include "type.h"
#include "expr.h"
#include "decl.h"
#include "stmt.h"
#include "scope.h"

struct stmt* stmt_create(
	stmt_t kind,
	struct decl* decl,
	struct expr* init_expr,
	struct expr* expr,
	struct expr* next_expr,
	struct stmt* body,
	struct stmt* else_body,
	struct stmt* next
)
{
	struct stmt* s = malloc(sizeof(*s));
	s->kind = kind;
	s->decl = decl;
	s->init_expr = init_expr;
	s->expr = expr;
	s->next_expr = next_expr;
	s->body = body;
	s->else_body = else_body;
	s->next = next;
	return s;
}

struct stmt* stmt_create_empty(stmt_t kind)
{
	return stmt_create(
		kind,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL
	);
}

/* Print a statement */

void stmt_print_indent(int indent)
{
	for (int i = 0; i < indent; i++)
		printf("\t");
}

void stmt_print_if_else(const struct stmt* s, int indent)
{
	stmt_print_indent(indent);
	printf("if (");
	expr_print(s->expr);
	printf(")\n");

	if (s->body)
	{
		if (s->body->kind == STMT_BLOCK)
			stmt_print(s->body, indent);
		else
			stmt_print(s->body, indent + 1);
	}

	if (s->else_body)
	{
		stmt_print_indent(indent);
		printf("else\n");
		if (s->else_body->kind == STMT_BLOCK)
			stmt_print(s->else_body, indent);
		else
			stmt_print(s->else_body, indent + 1);
	}
}

void stmt_print_for(const struct stmt* s, int indent)
{
	stmt_print_indent(indent);
	printf("for (");
	if (s->init_expr)
	{
		expr_print(s->init_expr);
		printf("; ");
	}
	else printf(";");
	if (s->next_expr)
	{
		expr_print(s->expr);
		printf("; ");
	}
	else printf(";");
	if (s->next_expr)
		expr_print(s->next_expr);
	printf(")\n");

	if (s->body->kind == STMT_BLOCK)
		stmt_print(s->body, indent);
	else
		stmt_print(s->body, indent + 1);
}

void stmt_print(const struct stmt* s, int indent)
{
	switch (s->kind)
	{
	case STMT_DECL:
		decl_print(s->decl, indent);
		break;
	case STMT_EXPR:
		stmt_print_indent(indent);
		expr_print(s->expr);
		printf(";\n");
		break;
	case STMT_IF_ELSE:
		stmt_print_if_else(s, indent);
		break;
	case STMT_FOR:
		stmt_print_for(s, indent);
		break;
	case STMT_PRINT:
		stmt_print_indent(indent);
		printf("print ");
		expr_print(s->expr);
		printf(";\n");
		break;
	case STMT_RETURN:
		stmt_print_indent(indent);
		printf("return");
		if (s->expr)
		{
			printf(" ");
			expr_print(s->expr);
		}
		printf(";\n");
		break;
	case STMT_BLOCK:
		stmt_print_indent(indent);
		printf("{\n");
		stmt_print(s->body, indent + 1);
		stmt_print_indent(indent);
		printf("}\n");
		break;
	}

	if (s->next)
		stmt_print(s->next, indent);
}

void stmt_resolve(const struct stmt* s)
{
	if (!s) return;
	switch (s->kind)
	{
	case STMT_DECL:
		decl_resolve(s->decl);
		break;
	case STMT_EXPR:
		expr_resolve(s->expr);
		break;
	case STMT_IF_ELSE:
		expr_resolve(s->expr);
		stmt_resolve(s->body);
		stmt_resolve(s->else_body);
		break;
	case STMT_FOR:
		expr_resolve(s->init_expr);
		expr_resolve(s->expr);
		expr_resolve(s->next_expr);
		stmt_resolve(s->body);
		break;
	case STMT_PRINT:
		expr_resolve(s->expr);
		break;
	case STMT_RETURN:
		expr_resolve(s->expr);
		break;
	case STMT_BLOCK:
		scope_enter();
		stmt_resolve(s->body);
		scope_exit();
		break;
	}
	stmt_resolve(s->next);
}

void stmt_typecheck(const struct stmt* s)
{
	if (!s) return;
	switch (s->kind)
	{
	case STMT_DECL:
		decl_typecheck(s->decl);
		break;
	case STMT_IF_ELSE:
	case STMT_FOR:
		expr_typecheck(s->init_expr);
		struct type* cond_type = expr_typecheck(s->expr);
		if (cond_type->kind != TYPE_BOOLEAN)
		{
			printf("Type Error | condition of if/for statement cannot be ");
			type_print(cond_type);
			printf(" (");
			expr_print(s->expr);
			printf(")\n");
			type_errors++;
		}
		expr_typecheck(s->next_expr);
		stmt_typecheck(s->body);
		stmt_typecheck(s->else_body);
		break;
	case STMT_BLOCK:
		stmt_typecheck(s->body);
		break;
	case STMT_EXPR:
	case STMT_PRINT:
	case STMT_RETURN:
		expr_typecheck(s->expr);
		break;
	}
	stmt_typecheck(s->next);
}
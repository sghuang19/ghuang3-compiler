#include <stdlib.h>
#include <stdio.h>
#include "stmt.h"

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
	printf(") {\n");

	if (s->body)
		stmt_print(s->body, indent + 1);

	stmt_print_indent(indent);
	printf("}");

	if (s->else_body)
	{
		printf(" else {\n");
		stmt_print(s->else_body, indent + 1);
		stmt_print_indent(indent);
		printf("}");
	}
}

void stmt_print_for(const struct stmt* s, int indent)
{
	stmt_print_indent(indent);
	printf("for (");
	expr_print(s->init_expr);
	printf("; ");
	expr_print(s->expr);
	printf("; ");
	expr_print(s->next_expr);
	printf(") {");

	stmt_print(s->body, indent + 1);

	stmt_print_indent(indent);
	printf("}");
}

void stmt_print(const struct stmt* s, int indent)
{
	switch (s->kind)
	{
	case STMT_DECL:
		decl_print(s->decl, indent);
		printf(";");
		break;
	case STMT_EXPR:
		expr_print(s->expr);
		printf(";");
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
		printf(";");
		break;
	case STMT_RETURN:
		stmt_print_indent(indent);
		printf("return ");
		expr_print(s->expr);
		printf(";");
		break;
	case STMT_BLOCK:
		stmt_print_indent(indent);
		printf("{\n");
		stmt_print(s->body, indent + 1);
		stmt_print_indent(indent);
		printf("}");
		break;
	}

	printf("\n");
	if (s->next)
		stmt_print(s->next, indent);
}
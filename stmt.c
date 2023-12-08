#include <stdlib.h>
#include <stdio.h>

#include "type.h"
#include "expr.h"
#include "decl.h"
#include "stmt.h"
#include "scope.h"
#include "scratch.h"
#include "label.h"

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
	extern struct type* rtype;
	struct type* expr_type;
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
		expr_typecheck(s->expr);
		break;
	case STMT_RETURN:
		expr_type = expr_typecheck(s->expr);
		if (!(rtype->kind == TYPE_VOID && expr_type == NULL) &&
			!type_equals(expr_type, rtype))
		{
			printf("Type Error | return type mismatch, expected ");
			type_print(rtype);
			printf(", got");
			if (s->expr)
			{
				printf(" ");
				expr_print(s->expr);
				printf(" (");
				type_print(expr_typecheck(s->expr));
				printf(")\n");
			}
			else
				printf(" nothing\n");
			type_errors++;
		}
		break;
	}
	stmt_typecheck(s->next);
}

void stmt_codegen(const struct stmt* s)
{
	if (!s) return;
	int top, end; // Labels
	extern const char* cur_func;

	switch (s->kind)
	{
	case STMT_DECL:
		decl_codegen(s->decl);
		break;
	case STMT_EXPR:
		if (s->expr)
		{
			expr_codegen(s->expr);
			scratch_free(s->expr->reg);
		}
		break;
	case STMT_IF_ELSE:
		top = label_create();
		end = label_create();

		printf("; if-else condition\n");
		if (s->expr)
		{
			expr_codegen(s->expr);
			printf("cmp $0, %s\n", scratch_name(s->expr->reg));
			scratch_free(s->expr->reg);
			printf("je %s\n", label_name(top)); // To false
		}

		printf("; if-else body\n");
		stmt_codegen(s->body);
		printf("jmp %s\n", label_name(end)); // To end

		printf("; if-else else body\n");
		printf("%s\n:", label_name(top)); // False label
		stmt_codegen(s->else_body);

		printf("%s\n:", label_name(end)); // End label
		break;
	case STMT_FOR:
		top = label_create();
		end = label_create();

		printf("; for-loop init expr\n");
		expr_codegen(s->init_expr);

		printf("%s\n:", label_name(top)); // Loop top

		printf("; for-loop expr\n");
		if (s->expr)
		{
			expr_codegen(s->expr);
			printf("cmp $0 %s\n", scratch_name(s->expr->reg));
			scratch_free(s->expr->reg);
			printf("je %s\n", label_name(end));
		}

		printf("; for-loop body\n");
		stmt_codegen(s->body);

		printf("; for-loop next expr\n");
		expr_codegen(s->next_expr);
		printf("jmp %s\n:", label_name(top));

		printf("%s\n:", label_name(end)); // Loop end
		break;
	case STMT_PRINT:
		// TODO: Call runtime library using register calling convention
		break;
	case STMT_RETURN:
		if (s->expr)
		{
			expr_codegen(s->expr);
			printf("movq %d, %%rax\n", s->expr->reg);
			scratch_free(s->expr->reg);
		}
		printf("jmp .%s_epilogue\n", cur_func);
		break;
	case STMT_BLOCK:
		stmt_codegen(s->body);
		break;
	}

	stmt_codegen(s->next);
}

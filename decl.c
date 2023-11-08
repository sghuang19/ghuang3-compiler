#include <stdlib.h>
#include <stdio.h>
#include "decl.h"
#include "type.h"
#include "symbol.h"
#include "scope.h"

struct decl* decl_create(char* name, struct type* type, struct expr* value, struct stmt* code, struct decl* next)
{
	struct decl* d = malloc(sizeof(*d));
	d->name = name;
	d->type = type;
	d->value = value;
	d->code = code;
	d->next = next;
	return d;
}

void decl_print(struct decl* d, int indent)
{
	if (!d) return;
	int i;

	for (i = 0; i < indent; i++)
		printf("\t");
	printf("%s: ", d->name);
	type_print(d->type);

	if (d->value)
	{
		printf(" = ");
		if (d->value->kind == EXPR_LIST)
		{
			printf("{");
			expr_print(d->value);
			printf("}");
		}
		else
			expr_print(d->value);
	}

	if (d->code)
	{
		printf(" = {\n");
		stmt_print(d->code, indent + 1);
		for (i = 0; i < indent; i++)
			printf("\t");
		printf("}\n");
	}
	else
		printf(";\n");

	decl_print(d->next, indent);
}

void decl_resolve(struct decl* d)
{
	if (!d) return;

	if (scope_lookup_current(d->name))
	{
		fprintf(stderr, "error: symbol '%s' already declared\n", d->name);
		res_errors++;
	}
	symbol_t kind;
	int which;
	if (cur_scope->level > 0)
	{
		kind = SYMBOL_LOCAL;
		which = cur_local++;
	}
	else
	{
		kind = SYMBOL_GLOBAL;
		which = cur_global++;
	}
	d->symbol = symbol_create(kind, d->type, d->name, which);
	scope_bind(d->name, d->symbol);

	if (d->type->kind == TYPE_ARRAY)
		expr_resolve(d->type->size);

	expr_resolve(d->value);
	if (d->code)
	{
		scope_enter();
		param_list_resolve(d->type->params);
		stmt_resolve(d->code);
		scope_exit();
	}

	decl_resolve(d->next);
}
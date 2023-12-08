#include <stdlib.h>
#include <stdio.h>

#include "stmt.h"
#include "expr.h"
#include "decl.h"
#include "type.h"
#include "param_list.h"
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

	if (scope_lookup_current(d->name) && d->type->kind != TYPE_FUNCTION)
	{
		fprintf(stderr, "Resolve Error | symbol '%s' already declared\n", d->name);
		res_errors++;
	}

	struct symbol* prev = scope_lookup(d->name);
	if (prev && prev->prototype && d->code)
	{
		fprintf(stderr, "Resolve Error | function '%s' already declared and defined\n", d->name);
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
	{
		expr_resolve(d->type->size);
		expr_resolve(d->type->subtype->size);
	}

	expr_resolve(d->value);
	if (d->code)
	{
		d->symbol->prototype = 0;
		scope_enter();
		param_list_resolve(d->type->params);
		stmt_resolve(d->code);
		scope_exit();
	}
	else
		d->symbol->prototype = 1;

	d->symbol->locals = cur_local;

	decl_resolve(d->next);
}

/** Types are type-checked in declarations only */
void type_typecheck(const struct type* t, const char* name)
{
	if (!t) return;
	if (t->kind == TYPE_ARRAY)
	{
		struct type* size_type = expr_typecheck(t->size);
		if (size_type->kind != TYPE_INTEGER)
		{
			printf("Type Error | size of array ('%s') cannot be ", name);
			type_print(size_type);
			printf(" (");
			expr_print(t->size);
			printf(")\n");
			type_errors++;
		}
		if (t->subtype->kind == TYPE_FUNCTION)
		{
			printf("Type Error | array of functions not allowed (%s)\n", name);
			type_errors++;
		}
		type_typecheck(t->subtype, name);
		return;
	}

	if (t->kind == TYPE_FUNCTION)
	{
		if (t->subtype->kind == TYPE_ARRAY)
		{
			printf("Type Error | function ('%s') cannot return array (", name);
			type_print(t->subtype);
			printf(")\n");
			type_errors++;
		}

		struct param_list* p = t->params;
		while (p)
		{
			if (p->type->kind == TYPE_FUNCTION)
			{
				printf("Type Error | functions '%s' cannot take function as parameter (", name);
				param_list_print(p);
				printf(")\n");
				type_errors++;
			}
			p = p->next;
		}
		return;
	}
}

struct type* rtype = NULL;

void decl_typecheck(struct decl* d)
{
	if (!d) return;

	// Global declarations must be constant
	if (d->symbol->kind == SYMBOL_GLOBAL && !expr_is_constant(d->value))
	{
		printf("Type Error | global variable ('%s') cannot be initialized "
			   "with non-constant expression ", d->name);
		if (d->value && d->value->kind == EXPR_LIST)
		{
			printf("{");
			expr_print(d->value);
			printf("}");
		}
		else
			expr_print(d->value);
		printf("\n");
		type_errors++;
	}

	type_typecheck(d->type, d->name);
	struct type* val_type = expr_typecheck(d->value);
	if (d->value)
	{
		if (d->type->kind == TYPE_ARRAY)
		{
			// Local arrays cannot be initialized
			if (d->symbol->kind == SYMBOL_LOCAL)
			{
				printf("Type Error | cannot initialize local array ('%s') ({", d->name);
				expr_print(d->value);
				printf("})\n");
				type_errors++;
			}

			// Check initializer type
			struct expr* e = d->value;
			while (e)
			{
				struct type* t = expr_typecheck(e->left);
				if (!type_equals(d->type->subtype, t))
				{
					printf("Type Error | cannot initialize array of ");
					type_print(d->type->subtype);
					printf(" ('%s') with element of ", d->name);
					type_print(t);
					printf(" (");
					expr_print(e->left);
					printf(")\n");
					type_errors++;
				}
				e = e->right;
			}
		}
		else if (!type_equals(d->type, val_type))
		{
			printf("Type Error | cannot initialize ");
			type_print(d->type);
			printf(" ('%s') with ", d->name);
			type_print(val_type);
			printf(" (");
			expr_print(d->value);
			printf(")\n");
			type_errors++;
		}
	}

	if (d->type->kind == TYPE_FUNCTION)
	{
		rtype = d->type->subtype;
		if (d->symbol->kind == SYMBOL_LOCAL)
		{
			printf("Type Error | cannot declare function ('%s') inside function\n", d->name);
			type_errors++;
		}
	}

	stmt_typecheck(d->code);
	decl_typecheck(d->next);
}

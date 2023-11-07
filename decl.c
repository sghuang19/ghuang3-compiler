#include <stdlib.h>
#include <stdio.h>
#include "decl.h"
#include "type.h"

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
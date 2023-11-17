#include <stdlib.h>
#include <stdio.h>

#include "param_list.h"
#include "type.h"
#include "expr.h"
#include "symbol.h"
#include "scope.h"

struct param_list* param_list_create(char* name, struct type* type, struct param_list* next)
{
	struct param_list* p = malloc(sizeof(*p));
	p->name = name;
	p->type = type;
	p->next = next;
	return p;
}

void param_list_print(struct param_list* p)
{
	if (!p) return;
	printf("%s: ", p->name);
	type_print(p->type);
	if (p->next)
	{
		printf(", ");
		param_list_print(p->next);
	}
}

void param_list_resolve(struct param_list* p)
{
	if (!p)
	{
		cur_param = 0;
		return;
	}
	if (scope_lookup_current(p->name))
	{
		fprintf(stderr, "Resolve Error | duplicate parameter '%s'\n", p->name);
		res_errors++;
	}
	struct symbol* s = symbol_create(SYMBOL_PARAM, p->type, p->name, cur_param++);
	scope_bind(p->name, s);
	if (p->type->kind == TYPE_ARRAY)
		expr_resolve(p->type->size);
	param_list_resolve(p->next);
}

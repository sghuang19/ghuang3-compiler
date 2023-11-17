#include <stdlib.h>
#include <stdio.h>

#include "scope.h"
#include "symbol.h"
#include "hash_table.h"

int res_errors = 0;
struct scope* cur_scope = NULL;
int cur_global, cur_param, cur_local = 0;

void scope_enter()
{
	struct scope* s = malloc(sizeof(*s));
	s->parent = cur_scope;
	s->table = hash_table_create(0, 0);
	s->level = cur_scope ? cur_scope->level + 1 : 0;
	cur_scope = s;
}

void scope_exit()
{
	if (!cur_scope) return;
	hash_table_delete(cur_scope->table);
	struct scope* tmp = cur_scope;
	cur_scope = cur_scope->parent;
	free(tmp);
}

void scope_bind(const char* name, struct symbol* s)
{
	hash_table_insert(cur_scope->table, name, s);
}

struct symbol* scope_lookup(const char* name)
{
	struct scope* tmp = cur_scope;
	while (tmp)
	{
		struct symbol* s = hash_table_lookup(tmp->table, name);
		if (s) return s;
		tmp = tmp->parent;
	}
	return NULL;
}

struct symbol* scope_lookup_current(const char* name)
{
	return hash_table_lookup(cur_scope->table, name);
}

void resolve_msg(struct symbol* s)
{
	printf("%s -> ", s->name);
	switch (s->kind)
	{
	case SYMBOL_LOCAL:
		printf("local @ %d\n", s->which);
		break;
	case SYMBOL_PARAM:
		printf("param @ %d\n", s->which);
		break;
	case SYMBOL_GLOBAL:
		printf("global @ %s\n", s->name);
		break;
	}
}

#include <stdio.h>
#include <stdlib.h>
#include "type.h"
#include "symbol.h"

struct symbol* symbol_create(symbol_t kind, struct type* type, char* name, int which)
{
	struct symbol* s = malloc(sizeof(*s));
	s->kind = kind;
	s->type = type;
	s->name = name;
	s->which = which;
	return s;
}

const char* symbol_codegen(const struct symbol* s)
{
	static char buf[16];
	char sign;
	switch (s->kind)
	{
	case SYMBOL_LOCAL:
		sign = '-';
		break;
	case SYMBOL_PARAM:
		sign = ' ';
		break;
	case SYMBOL_GLOBAL:
		return s->name;
	}
	unsigned long length = snprintf(buf, sizeof(buf), "%c%d(%%rbp)", sign, 8 * (s->which + 1));
	if (length > sizeof(buf))
	{
		fprintf(stderr, "CodeGen Error | too many parameters, cannot generate symbol\n");
		extern int codegen_errors;
		codegen_errors++;
	}
	return buf;
}

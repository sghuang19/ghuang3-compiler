#ifndef SYMBOL_H
#define SYMBOL_H

typedef enum
{
	SYMBOL_LOCAL,
	SYMBOL_PARAM,
	SYMBOL_GLOBAL
} symbol_t;

struct symbol
{
	symbol_t kind;
	struct type* type;
	char* name;
	int which;
	int prototype; // 1 if function prototype, 0 otherwise
	int locals; // number of local variables
};

struct symbol* symbol_create(symbol_t kind, struct type* type, char* name, int which);

const char* symbol_codegen(const struct symbol* s);

#endif

#include <stdlib.h>
#include <stdio.h>

#include "type.h"
#include "expr.h"
#include "param_list.h"

/* Creating the types */

struct type* type_create(type_t kind)
{
	struct type* t = malloc(sizeof(*t));
	t->kind = kind;
	return t;
}

struct type* type_create_array(struct type* subtype, struct expr* size)
{
	struct type* t = type_create(TYPE_ARRAY);
	t->subtype = subtype;
	t->size = size;
	return t;
}

struct type* type_create_func(struct type* rtype, struct param_list* params)
{
	struct type* t = type_create(TYPE_FUNCTION);
	t->subtype = rtype;
	t->params = params;
	return t;
}

/* Printing the types */

void type_print_primitive(const struct type* t)
{
	switch (t->kind)
	{
	case TYPE_INTEGER:
		printf("integer");
		break;
	case TYPE_FLOAT:
		printf("float");
		break;
	case TYPE_CHAR:
		printf("char");
		break;
	case TYPE_VOID:
		printf("void");
		break;
	case TYPE_BOOLEAN:
		printf("boolean");
		break;
	case TYPE_STRING:
		printf("string");
		break;
	default:
		/* Compound types handled in type_print() */
		break;
	}
}

void type_print(const struct type* t)
{
	if (!t) return;
	switch (t->kind)
	{
	case TYPE_ARRAY:
		printf("array [");
		if (t->size != NULL)
			expr_print(t->size);
		printf("] ");
		type_print(t->subtype);
		break;
	case TYPE_FUNCTION:
		printf("function ");
		type_print(t->subtype);
		printf(" (");
		param_list_print(t->params);
		printf(")");
		break;
	default:
		/* Primitive types */
		type_print_primitive(t);
		break;
	}
}

/* Typechecking */

int type_equals(const struct type* t1, const struct type* t2)
{
	if (!t1 && !t2) return 1; // Both are NULL
	if (!t1 || !t2) return 0; // One is NULL, the other is not

	if (t1->kind != t2->kind)
		return 0;
	switch (t1->kind)
	{
	case TYPE_ARRAY:
		return type_equals(t1->subtype, t2->subtype);
	case TYPE_FUNCTION:
		if (!type_equals(t1->subtype, t2->subtype))
			return 0;
		struct param_list* p1 = t1->params;
		struct param_list* p2 = t2->params;
		while (p1 && p2)
		{
			if (!type_equals(t1->params->type, t2->params->type))
				return 0;
			p1 = p1->next;
			p2 = p2->next;
		}
		return !p1 && !p2 ? 1 : 0;
	default:
		return 1;
	}
}

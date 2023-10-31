#ifndef TYPE_H
#define TYPE_H

#include "param_list.h"

typedef enum
{
	TYPE_VOID,
	TYPE_BOOLEAN,
	TYPE_CHAR,
	TYPE_INTEGER,
	TYPE_STRING,
	TYPE_ARRAY,
	TYPE_FUNCTION,
} type_t;

struct type
{
	type_t kind;
	struct param_list* params;
	struct type* subtype;
};

/* Creating types */
struct type* type_create(type_t kind);
struct type* type_create_array(struct type* subtype);
struct type* type_create_func(struct type* rtype, struct param_list* params);

void type_print(struct type* t);

#endif

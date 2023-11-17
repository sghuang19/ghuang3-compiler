#ifndef TYPE_H
#define TYPE_H

typedef enum
{
	TYPE_VOID,
	TYPE_BOOLEAN,
	TYPE_CHAR,
	TYPE_INTEGER,
	TYPE_FLOAT,
	TYPE_STRING,
	TYPE_ARRAY,
	TYPE_FUNCTION,
} type_t;

extern int type_errors;

struct type
{
	type_t kind;
	struct type* subtype;
	struct expr* size;
	struct param_list* params;
};

/* Creating types */
struct type* type_create(type_t kind);
struct type* type_create_array(struct type* subtype, struct expr* size);
struct type* type_create_func(struct type* rtype, struct param_list* params);

void type_print(const struct type* t);

/** @return 1 if two types are equal, 0 otherwise. */
int type_equals(const struct type* t1, const struct type* t2);

#endif

#ifndef PARAM_LIST_H
#define PARAM_LIST_H

struct param_list
{
	char* name;
	struct type* type;
	struct param_list* next;
};

struct param_list* param_list_create(char* name, struct type* type, struct param_list* next);
void param_list_print(struct param_list* p);
void param_list_resolve(struct param_list* p);
struct expr;
/** @return 1 if types of params and args match, 0 otherwise. */
int param_typecheck(const struct param_list* params, const struct expr* args);

#endif

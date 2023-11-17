#ifndef _SCOPE_H_
#define _SCOPE_H_

struct symbol; // Forward declaration

struct scope
{
	int level;
	struct scope* parent;
	struct hash_table* table;
};

extern int res_errors;
extern struct scope* cur_scope;
extern int cur_global;
extern int cur_param;
extern int cur_local;

void scope_enter();
void scope_exit();
void scope_bind(const char* name, struct symbol* s);
struct symbol* scope_lookup(const char* name);
struct symbol* scope_lookup_current(const char* name);
void resolve_msg(struct symbol* s);

#endif

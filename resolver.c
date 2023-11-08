#include "resolver.h"
#include "scope.h"

int resolve(struct decl* root)
{
	scope_enter();
	decl_resolve(root);
	scope_exit();
	return res_errors;
}

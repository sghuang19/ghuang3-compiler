#include <stdio.h>
#include "typechecker.h"
#include "codegen.h"

int codegen_errors = 0;

int codegen(struct decl* d)
{
	int type_errors = typecheck(d); // name resolution and type checking
	if (type_errors)
	{
		fprintf(stderr, "CodeGen Error | Type errors occurred, aborting code generation\n");
		return type_errors;
	}

	decl_codegen(d);
	return codegen_errors;
}
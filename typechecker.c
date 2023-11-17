#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "typechecker.h"
#include "resolver.h"

int type_errors = 0;

int typecheck(struct decl* root)
{
	// Save stdout
	fflush(stdout);
	int stdout_saved = dup(STDOUT_FILENO);
	// Suppress output from resolver
	int null_fd = open("/dev/null", O_WRONLY);
	dup2(null_fd, STDOUT_FILENO);

	int resolve_errors = resolve(root);
	if (resolve_errors)
		fprintf(stderr, "Resolve Error | Resolve errors found, aborting typechecking\n");
	else
	{
		// Redirect stdout to stderr for printing types and expressions
		fflush(stdout);
		dup2(STDERR_FILENO, STDOUT_FILENO);
		decl_typecheck(root);
	}

	// Restore stdout
	fflush(stdout);
	dup2(stdout_saved, STDOUT_FILENO);
	close(stdout_saved);
	return resolve_errors ? resolve_errors : type_errors;
}

#include <stdlib.h>
#include <stdio.h>
#include "label.h"
#include "scratch.h"

#define LABEL_LENGTH 8
#define MAX_LABEL 99999
static char name[LABEL_LENGTH];

extern int codegen_errors;

static int label_count = 0;

int label_create()
{
	if (label_count++ == MAX_LABEL)
	{
		fprintf(stderr, "CodeGen Error | Maximum number of labels reached\n");
		codegen_errors++;
		exit(EXIT_FAILURE);
	}
	return label_count;
}

char* label_name(int index)
{
	sprintf(name, ".L%d", index);
	return name;
}

static int string_count = 0;

/** @return a static string representation the label, e.g. .S1 */
char* string_name(int index)
{
	sprintf(name, ".S%d", index);
	return name;
}

int string_create(const char* string, int reg)
{
	if (string_count++ == MAX_LABEL)
	{
		fprintf(stderr, "CodeGen Error | Maximum number of strings reached\n");
		codegen_errors++;
		exit(EXIT_FAILURE);
	}
	printf("\n");
	printf(".data\n");
	printf("%s: ", string_name(string_count));
	printf(".string \"%s\"\n", string);
	printf(".text\n");
	printf("leaq %s, %s", string_name(string_count), scratch_name(reg));
	return string_count;
}

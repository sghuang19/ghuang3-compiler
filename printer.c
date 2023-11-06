#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "printer.h"
#include "parser.h"

void print(struct decl* d)
{
	decl_print(d, 0);
}

int fprint(struct decl* d, FILE* fp)
{
	int fd = fileno(fp);
	int saved_stdout = dup(STDOUT_FILENO);
	if (dup2(fd, STDOUT_FILENO) == -1)
	{
		fprintf(stderr, "Failed to redirect stdout to file\n");
		return 1;
	}
	print(d);
	dup2(saved_stdout, STDOUT_FILENO);
	close(saved_stdout);
	return 0;
}

int print_and_compare(char* filename, FILE* fp)
{
	// FIXME: "syntax error at line 1 near ''" when printing the second file
	// TODO: Suppress output from scanner and printer
	// Parse the input file
	struct decl* d1 = parse(fp);
	if (d1 == NULL)
	{
		printf("Failed to parse file %s before printing\n", filename);
		return EXIT_FAILURE;
	}
	printf("\n======== Printing started ========\n\n");
	print(d1);

	// Construct filenames
	char* ext_out = ".out";
	char* ext_tmp = ".tmp";
	char* output = malloc(strlen(filename) + strlen(ext_out) + 1);
	char* temp = malloc(strlen(filename) + strlen(ext_tmp) + 1);
	if (output == NULL || temp == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for output filename\n");
		return EXIT_FAILURE;
	}
	strcpy(output, filename);
	strcat(output, ext_out);
	strcpy(temp, filename);
	strcat(temp, ext_tmp);

	// Open output file for printing
	FILE* fp_out = fopen(output, "w+");
	if (fp_out == NULL)
	{
		fprintf(stderr, "Failed to open file %s for printing\n", output);
		return EXIT_FAILURE;
	}

	// Print the AST to the output file
	if (fprint(d1, fp_out))
	{
		fprintf(stderr, "Failed to print file %s\n", filename);
		fclose(fp_out);
		return EXIT_FAILURE;
	}
	else
	{
		printf("Successfully printed to file %s\n", output);
	}

	// Parse the output file
	struct decl* d2 = parse(fp_out);
	fclose(fp_out);
	if (d2 == NULL)
	{
		fprintf(stderr, "Failed to parse file %s after printing\n", output);
		return EXIT_FAILURE;
	}

	// Open temp file for printing
	FILE* fp_tmp = fopen(temp, "w");
	if (fp_tmp == NULL)
	{
		fprintf(stderr, "Failed to open file %s for printing\n", temp);
		return EXIT_FAILURE;
	}

	// Print the AST to the temp file
	if (fprint(d2, fp_tmp))
	{
		fprintf(stderr, "Failed to print to file %s\n", temp);
		return EXIT_FAILURE;
	}
	else
	{
		printf("Successfully printed to file %s\n", temp);
	}

	// Compare the output and temp files
	char* command = malloc(strlen(output) + strlen(temp) + 7);
	sprintf(command, "diff %s %s", output, temp);
	int ret = system(command);
	switch (ret)
	{
	case -1:
		fprintf(stderr, "Failed to execute diff command\n");
		return EXIT_FAILURE;
	case 0:
		printf("Successfully printed and verified\n");
		remove(temp);
		return EXIT_SUCCESS;
	default:
		fprintf(stderr, "Successfully printed but failed to verify\n");
		return EXIT_FAILURE;
	}
}

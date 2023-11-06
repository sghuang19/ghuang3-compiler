#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "encoder.h"
#include "scanner.h"
#include "parser.h"
#include "printer.h"

void usage(int exit_code)
{
	printf("Usage of bminor.\n");
	exit(exit_code);
}

int main(int argc, char* argv[])
{
	// Parse command line arguments
	char* option, * filename;
	switch (argc)
	{
	case 1:
		usage(EXIT_FAILURE);
		break;
	case 2:
		if (strcmp(argv[1], "--help") == 0)
			usage(EXIT_SUCCESS);
		else usage(EXIT_FAILURE);
		break;
	case 3:
		option = argv[1];
		filename = argv[2];
		break;
	default:
		fprintf(stderr, "Too many arguments.\n");
		usage(EXIT_FAILURE);
		break;
	}

	// Open input file
	FILE* fp = fopen(filename, "r");
	if (fp == NULL)
	{
		fprintf(stderr, "Failed to open file %s\n", filename);
		return EXIT_FAILURE;
	}

	// Perform the requested operation
	if (strcmp(option, "--encode") == 0)
	{
		if (decode(fp) == 0)
			return EXIT_SUCCESS;
	}
	else if (strcmp(option, "--scan") == 0)
	{
		if (scan(fp) == 0)
			return EXIT_SUCCESS;
	}
	else if (strcmp(option, "--parse") == 0)
	{
		if (parse(fp) != NULL)
			return EXIT_SUCCESS;
	}
	else if (strcmp(option, "--print") == 0)
	{
		return print_and_compare(filename, fp);
	}
	else
	{
		fprintf(stderr, "Unknown option '%s'\n", option);
		usage(EXIT_FAILURE);
	}

	//Error message
	fprintf(stderr, "Failed to %s file %s\n", option + 2, filename);
	return EXIT_FAILURE;
}

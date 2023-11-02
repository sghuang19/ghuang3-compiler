#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "encoder.h"
#include "scanner.h"

void usage(int exit_code)
{
	printf("Usage of bminor.\n");
	exit(exit_code);
}

int main(int argc, char* argv[])
{
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

	if (strcmp(option, "--encode") == 0)
	{
		if (decode(filename) == 0)
			return EXIT_SUCCESS;
	}
	else if (strcmp(option, "--scan") == 0)
	{
		if (scan(filename) == 0)
			return EXIT_SUCCESS;
	}
	else
	{
		fprintf(stderr, "Unknown option '%s'\n", option);
		usage(EXIT_FAILURE);
	}

	fprintf(stderr, "Failed to %s file %s\n", option + 2, filename);
	return EXIT_FAILURE;
}

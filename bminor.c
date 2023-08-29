#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "encoder.h"

void usage(int exit_code)
{
	printf("Usage of bminor.\n");
	exit(exit_code);
}

int main(int argc, char* argv[])
{

	if (argc == 1)
		usage(1);

	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (strcmp(argv[i], "--help") == 0)
				usage(0);
			else if (strcmp(argv[i], "--encode") == 0)
			{
				char* filename = argv[++i];
				if (filename)
				{
					if (decode(filename) == 0)
					{
//						printf("Successfully decoded file %s\n", filename);
						return EXIT_SUCCESS;
					} else
					{
						fprintf(stderr, "Failed to decode file %s\n", filename);
						return EXIT_FAILURE;
					}
				}
				else
				{
					fprintf(stderr, "Missing filename to be encoded\n");
					return EXIT_FAILURE;
				}
			}
			else
			{
				fprintf(stderr, "Unknown option '%s'\n", argv[i]);
				usage(1);
			}
		}
	}

	return EXIT_SUCCESS;
}

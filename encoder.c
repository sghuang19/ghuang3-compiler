#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "encoder.h"

int is_hex(char c)
{
	return (c >= '0' && c <= '9') ||
		   (c >= 'A' && c <= 'F') ||
		   (c >= 'a' && c <= 'f');
}

int string_decode(const char* es, char* s)
{
	// Check for start and end quotes
	size_t es_len = strlen(es);
	if (es_len < 2 || *es != '"' || *(es + es_len - 1) != '"')
	{
		fprintf(stderr, "Invalid string: does not start or end with a quote\n");
		return 1;
	}

	const char* es_ptr = es + 1;
	char* s_ptr = s;
	size_t len = 0;

	while (es_ptr < es + es_len - 1)
	{
		// Check current length of decoded string
		if (len >= MAX_STRING_LEN)
		{
			fprintf(stderr, "Invalid string: too long\n");
			return 1;
		}

		// Check for printable characters
		if (*es_ptr < 32 || *es_ptr > 126)
		{
			fprintf(stderr, "Invalid string: invalid character\n");
			return 1;
		}

		// Check for escape sequences
		if (*es_ptr == '\\')
		{
			es_ptr++;
			switch (*es_ptr)
			{
			case 'a':
				*s_ptr = '\a';
				break;
			case 'b':
				*s_ptr = '\b';
				break;
			case 'e':
				*s_ptr = 27;
				break;
			case 'f':
				*s_ptr = '\f';
				break;
			case 'n':
				*s_ptr = '\n';
				break;
			case 'r':
				*s_ptr = '\r';
				break;
			case 't':
				*s_ptr = '\t';
				break;
			case 'v':
				*s_ptr = '\v';
				break;
			case '\\':
				*s_ptr = '\\';
				break;
			case '\'':
				*s_ptr = '\'';
				break;
			case '"':
				if (*(es_ptr + 1) == '\0')
				{
					fprintf(stderr, "Invalid string: escape sequence at end of string\n");
					return 1;
				}
				*s_ptr = '"';
				break;
			case '0':
				es_ptr++;
				if (*es_ptr == 'x')
				{
					es_ptr++;
					if (is_hex(*es_ptr) && is_hex(*(es_ptr + 1)))
					{
						char hex[2] = { *es_ptr, *(es_ptr + 1) };
						char val = (char)strtol(hex, NULL, 16);
						if (val < 0 || val > 127)
						{
							fprintf(stderr, "Invalid string: hex character not in ASCII\n");
							return 1;
						}
						*s_ptr = val;
						es_ptr += 1;
					}
					else
					{
						fprintf(stderr, "Invalid string: invalid hex escape sequence\n");
						return 1;
					}
				}
				break;
			default:
				fprintf(stderr, "Invalid string: invalid escape sequence\n");
				return 1;
			}
			es_ptr++;
			s_ptr++;
			len++;
			continue;
		}

		*s_ptr = *es_ptr;

		es_ptr++;
		s_ptr++;
		len++;
	}

	return 0;
}

int string_encode(const char* s, char* es)
{
	const char* s_ptr = s;
	char* es_ptr = es;

	*es_ptr = '"';
	es_ptr++;

	while (*s_ptr != '\0')
	{
		// Handle printable characters
		if (*s_ptr >= 32 && *s_ptr <= 126)
		{
			if (*s_ptr == '"' || *s_ptr == '\\')
			{
				*es_ptr = '\\';
				es_ptr++;
			}
			*es_ptr = *s_ptr;
			es_ptr++;
			s_ptr++;
			continue;
		}

		// Handle escape sequences
		*es_ptr = '\\';
		es_ptr++;
		switch (*s_ptr)
		{
		case '\a':
			*es_ptr = 'a';
			break;
		case '\b':
			*es_ptr = 'b';
			break;
		case 27:
			*es_ptr = 'e';
			break;
		case '\f':
			*es_ptr = 'f';
			break;
		case '\n':
			*es_ptr = 'n';
			break;
		case '\r':
			*es_ptr = 'r';
			break;
		case '\t':
			*es_ptr = 't';
			break;
		case '\v':
			*es_ptr = 'v';
			break;

		default:
			sprintf(es_ptr, "0x%X", *s_ptr);
			printf("Hex: %s\n", es_ptr);
			es_ptr += 3;
			break;
		}
		es_ptr++;
		s_ptr++;
	}

	*es_ptr = '"';
	es_ptr++;
	*es_ptr = '\0';
	return 0;
}

int decode(FILE* fp)
{
	// Find the size of the file
	fseek(fp, 0, SEEK_END);
	unsigned long file_size = ftell(fp);
	if (file_size > (MAX_STRING_LEN * 5 + 2) * sizeof(char))
	{
		fprintf(stderr, "Invalid string: too long\n");
		fclose(fp);
		return 1;
	}
	rewind(fp);

	// Allocate memory for the file content
	// Reserve space for \0
	char* file_content = (char*)malloc(file_size + sizeof(char));
	if (file_content == NULL)
	{
		perror("Could not allocate memory");
		fclose(fp);
		return 1;
	}

	// Read the file content into the allocated memory
	size_t chars_read = fread(file_content, sizeof(char), file_size, fp);
	file_content[chars_read] = '\0';

	char s[MAX_STRING_LEN + 1] = { 0 };

	if (string_decode(file_content, s) == 0)
	{
		char es[chars_read];
		memset(es, 0, chars_read);
		string_encode(s, es);
		printf("%s\n", es);
		return 0;
	}
	else
		return 1;

}

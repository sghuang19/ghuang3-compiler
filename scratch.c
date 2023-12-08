#include <stdio.h>
#include "scratch.h"

// %rbx, %r10, %r11, %r12, %r13, %r14, %r15
int regs[7] = { 0, 0, 0, 0, 0, 0, 0 };

extern int codegen_errors;

int scratch_alloc()
{
	int i;
	for (i = 0; i < 7; i++)
	{
		if (regs[i] == 0)
		{
			regs[i] = 1;
			return i;
		}
	}
	fprintf(stderr, "Codegen Error | Ran out of scratch registers\n");
	codegen_errors++;
	return -1;
}

void scratch_free(int r)
{
	if (r >=0 && r <= 6)
		regs[r] = 0;
}

const char* scratch_name(int r)
{
	switch (r)
	{
	case 0:
		return "%rbx";
	case 1:
		return "%r10";
	case 2:
		return "%r11";
	case 3:
		return "%r12";
	case 4:
		return "%r13";
	case 5:
		return "%r14";
	case 6:
		return "%r15";
	default:
		fprintf(stderr, "CodeGen Error | Invalid scratch register\n");
		codegen_errors++;
		return NULL;
	}
}

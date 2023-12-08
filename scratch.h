#ifndef SCRATCH_H
#define SCRATCH_H

/**
 * Allocates a scratch register. %rbx and %r1{0..5} are used
 * @return the index of the scratch register, -1 if none are available
 * */
int scratch_alloc();

/** Frees a scratch register. */
void scratch_free(int r);

/** @return the name of a scratch register, with % */
const char* scratch_name(int r);

#endif

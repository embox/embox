/**
 * \file sys.c
 *
 * \date Jun 09, 2009
 * \author Eldar Abusalimov
 * \author Alexey Fomin
 */

#include "sys.h"

void context_save(CPU_CONTEXT * pcontext) {
	// TODO see disassembler output
	__asm__ __volatile__(
			"mov %0, %%o0\n\t"
			"ta 5\n\t"
			"nop\n\t"::
			"r" (pcontext):
			"%o0");

}

void context_restore(CPU_CONTEXT * pcontext) {
	__asm__ __volatile__(
			"mov %0, %%o0\n\t"
			"ta 6\n\t"
			"nop\n\t"::
			"r" (pcontext):
			"%o0");
}

static CPU_CONTEXT context;

int sys_exec(EXEC_FUNC f, int argc, char **argv) {
	volatile static BOOL started;
	int ret = -2;

	started = FALSE;
	context_save(&context);
	{
		if (!started) {
			started = TRUE;
			ret = f(argc, argv);
		} else {
			return ret;
		}
	}
	context_restore(&context);

	// we'll never reach this line
	return ret;
}

void sys_interrupt() {
	//context_restore(&context);
}


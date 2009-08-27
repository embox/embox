/**
 * \file sys.c
 *
 * \date Jun 09, 2009
 * \author Eldar Abusalimov
 * \author Alexey Fomin
 */

#include "kernel/sys.h"

void context_save(CPU_CONTEXT * pcontext) {
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
volatile static BOOL started = FALSE;

int sys_exec_start(EXEC_FUNC f, int argc, char **argv) {
	if (started) {
		return -3;
	}

	int ret = -2;

	context_save(&context);

	if (!started) {
		started = TRUE;
		ret = f(argc, argv);
	}
	started = FALSE;

	return ret;
}

void sys_exec_stop() {
	if (!started) {
		return;
	}
	context_restore(&context);
}

void sys_halt() {
	//todo must be trap
}


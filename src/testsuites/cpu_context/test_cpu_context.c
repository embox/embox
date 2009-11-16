/**
 * \file test_cpu_context.c
 *
 *  \date Jan 26, 2009
 *  \author Anton Bondarev
 */
#include "common.h"
#include "asm/cpu_context.h"
#include "kernel/sys.h"
#include "express_tests.h"

DECLARE_EXPRESS_TEST("CPU context", exec);

/**
 * in this trap function contex_save takes place
 * this function save all procesor context to pointed memory
 * then call function context_restore
 * @return 0 if success
 */
static int exec() {
	static CPU_CONTEXT context;
	volatile static BOOL started;

	started = FALSE;
	context_save(&context);
	{
		if (!started) {
			started = TRUE;
		} else {
			return 0;
		}
	}
	context_restore(&context);

	// we'll never reach this line
	return -1;
}

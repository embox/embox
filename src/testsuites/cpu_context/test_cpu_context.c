/**
 * \file test_cpu_context.c
 *
 *  \date Jan 26, 2009
 *  \author Anton Bondarev
 */

#include "autoconf.h"
#include "common.h"
#include "asm/cpu_context.h"
#include "kernel/sys.h"
#include "express_tests.h"

DECLARE_EXPRESS_TEST("CPU context", exec, TEST_CPU_CONTEXT_ON_BOOT_ENABLE, NULL);

/**
 * in this trap function contex_save takes place
 * this function save all processor context to pointed memory
 * then call function context_restore
 * @return 0 if success
 */
static int exec() {
	static CPU_CONTEXT context;
	volatile static bool started;

	started = false;
	context_save(&context);
	{
		if (!started) {
			started = true;
		} else {
			return 0;
		}
	}
	context_restore(&context);

	// we'll never reach this line
	return -1;
}

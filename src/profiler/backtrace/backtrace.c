/**
 * @file
 * @brief TODO: brief
 *
 * @date 19 Apr, 2012
 * @author Bulychev Anton
 */

#include "stackframe.h"
#include <types.h>
#include <stdio.h>

// TODO: Delete it
struct stackframe {
	void* fp;
	void* pc;
};

void backtrace_fd(void) {
	struct stackframe f;
	stackframe_set_current(&f);

	printk("\nBacktrace:\n");
	stackframe_print(&f);
	while (stackframe_set_prev(&f)) {
		stackframe_print(&f);
	}
	//printk("last pc: 0x%p\n", f.pc);
	printk("\n");
}

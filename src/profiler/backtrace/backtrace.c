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

struct stackframe {
	void* fp;
	void* pc;
};

void backtrace_fd(void) {
	struct stackframe f;
	stackframe_set_current(&f);

	printf("\nBacktrace:\n");
	stackframe_print(&f);
	//printk("[%d] pc == 0x%p fp == %p\n", depth++, f.pc, f.fp);
	while (stackframe_set_prev(&f)) {

		stackframe_print(&f);
		//printk("[%d] pc == 0x%p fp == %p\n", depth++, f.pc, f.fp);
	}
	printf("\n");
}

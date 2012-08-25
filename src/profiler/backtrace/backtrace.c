/**
 * @file
 * @brief TODO: brief
 *
 * @date 19 Apr, 2012
 * @author Bulychev Anton
 */

#include <stdio.h>
#include "backtrace.h"

void backtrace_fd(void) {
	stack_iter_t f;
	stack_iter_current(&f);

	printk("\nStart backtrace:\n\n");
	stack_iter_print(&f);
	while (stack_iter_next(&f)) {
		stack_iter_print(&f);
	}
	printk("End backtrace\n\n");
}

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
	int k = 1;
	printk("\n\nBacktrace:\n\n");
	printk("  #         fp         pc\n");
	printk("--- ---------- ----------\n");

	/* Counting frames */
	stack_iter_current(&f);
	while (stack_iter_next(&f)) {
		k++;
	}

	/* Printing frames */
	stack_iter_current(&f);
	do {
		printk("%3d 0x%08x 0x%08x\n",
				k--, (uint32_t) stack_iter_get_fp(&f),
				(uint32_t) stack_iter_get_retpc(&f));
	} while (stack_iter_next(&f));

	printk("\n");
}

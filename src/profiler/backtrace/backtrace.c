/**
 * @file
 * @brief TODO: brief
 *
 * @date 19 Apr, 2012
 * @author Bulychev Anton
 */

#include <stdio.h>
#include <debug/symbol.h>
#include "backtrace.h"

void backtrace_fd(void) {
	stack_iter_t f;
	const struct symbol *s;
	void *fp, *pc;
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
		fp = stack_iter_get_fp(&f);
		pc = stack_iter_get_retpc(&f);
		s = symbol_lookup(pc);
		if (s == NULL) {
			printk("%3d 0x%08x 0x%08x\n",
				k--, (uint32_t) fp, (uint32_t) pc);
		} else {
			printk("%3d 0x%08x 0x%08x %s+0x%08x\n",
				k--, (uint32_t) fp, (uint32_t) pc,
				s->name, (uint32_t) pc - (uint32_t) s->addr);
		}
	} while (stack_iter_next(&f));

	printk("\n");
}

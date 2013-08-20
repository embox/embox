/**
 * @file
 * @brief TODO: brief
 *
 * @date 19 Apr, 2012
 * @author Bulychev Anton
 */

#include <stddef.h>
#include <string.h>
#include <kernel/printk.h>
#include <debug/backtrace.h>
#include <debug/symbol.h>
#include <math.h>
#include <module/embox/arch/stackframe.h>

extern void stack_iter_current(stack_iter_t *);
extern int stack_iter_next(stack_iter_t *);
extern void *stack_iter_get_fp(stack_iter_t *);
extern void *stack_iter_get_retpc(stack_iter_t *);

void backtrace(void) {
	stack_iter_t f;
	int depth = 0;

	/* Counting frames */
	stack_iter_current(&f);
	do {
		depth++;
	} while (stack_iter_next(&f));

	printk("\n\nBacktrace:\n\n");
	printk("     sp        pc         func + offset\n");
	printk("     --------  --------  --------------------------\n");

	/* Printing frames */
	stack_iter_current(&f);
	do {
		const struct symbol *s;

		void *fp = stack_iter_get_fp(&f);
		void *pc = stack_iter_get_retpc(&f);

		printk("%3d  %08x  %08x",
			depth--, (unsigned) fp, (unsigned) pc);

		s = symbol_lookup(pc);
		if (s) {
			ptrdiff_t offset = (char *) pc - (char *) s->addr;
			printk("  <%s+0x%x>",
					offset >= 0 ? s->name : "__unknown__",
					(unsigned) offset);
		}

		printk("\n");
	} while (stack_iter_next(&f));

	printk("\n");
}

/**
 * @file
 * @brief TODO: brief
 *
 * @date 16 Aug, 2012
 * @author Bulychev Anton
 */

#include <stdio.h>
#include <asm/traps.h>
#include "stack_iter.h"

void stack_iter_current(stack_iter_t *f) {
	f->fp = __builtin_frame_address(0);
	f->pc = __builtin_return_address(0);
}

int stack_iter_next(stack_iter_t *f) {
	extern void irq_handler_call_pointer(void);
	extern void exception_handler_call_pointer(void);

	void **p = f->fp;
	p = *p;

	if (f->pc == irq_handler_call_pointer || f->pc == exception_handler_call_pointer) {
		// Through interruption
		pt_regs_t *r = (pt_regs_t *) p;
		f->fp = (void *) r->ebp;
		f->pc = (void *) r->eip;
		printk("Through interruption: \n");
	} else {
		if (*p == NULL) {
			f->fp = NULL;
			//f->pc = NULL;
			f->pc = *(p+1);
			return 0;
		} else {
			f->fp = p;
			f->pc = *(p+1);
		}
	}

	return 1;
}

void stack_iter_print(stack_iter_t *f) {
	printk("frame_address = 0x%08x, return_address = 0x%08x  \n", (uint32_t) f->fp, (uint32_t) f->pc);
}

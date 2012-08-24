/**
 * @file
 * @brief TODO: brief
 *
 * @date 16 Aug, 2012
 * @author Bulychev Anton
 */

#include <stdio.h>
#include <asm/traps.h>

struct stackframe {
	void* fp;
	void* pc;
};

void stackframe_set_current(struct stackframe* f) {
	f->fp = __builtin_frame_address(0);
	f->pc = __builtin_return_address(0);
}

// TODO: May be add type of previous: through interruption or not?
int stackframe_set_prev(struct stackframe *f) {
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

void stackframe_print(struct stackframe* f) {
	printk("fp = 0x%p, pc = 0x%p  \n", f->fp, f->pc);
}

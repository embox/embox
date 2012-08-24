/**
 * @file
 * @brief TODO: brief
 *
 * @date 24 Aug, 2012
 * @author Bulychev Anton
 */

#include <stdio.h>
#include <asm/stack.h>

struct stackframe {
	void* fp;
	void* pc;
};

// TODO: __builtin_extract_return_address!!!
void stackframe_set_current(struct stackframe* f) {
	f->fp = __builtin_frame_address(0);
	f->pc = __builtin_return_address(0);
}

// TODO: May be add type of previous: through interruption or not?
// TODO: __builtin_extract_return_address!!!
int stackframe_set_prev(struct stackframe *f) {
	struct stack_frame* sf = f->fp;
	f->fp = sf->reg_window.fp;
	if (f->fp == NULL) {
		f->pc = NULL;
		return 0;
	}
	f->pc = sf->reg_window.ret_pc;

	return 1;
}

void stackframe_print(struct stackframe* f) {
	printk("fp = 0x%p, pc = 0x%p  \n", f->fp, f->pc);
}

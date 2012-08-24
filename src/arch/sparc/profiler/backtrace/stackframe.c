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
	struct stack_frame* sf;
};

// TODO: __builtin_extract_return_address!!!
void stackframe_set_current(struct stackframe* f) {
	f->fp = __builtin_frame_address(0);
	f->pc = __builtin_return_address(0);
	f->sf = NULL;
}

// TODO: May be add type of previous: through interruption or not?
// TODO: __builtin_extract_return_address!!!
int stackframe_set_prev(struct stackframe *f) {
	f->sf = f->fp;
	f->fp = f->sf->reg_window.fp;
	if (f->fp == NULL) {
		f->pc = NULL;
		f->sf = NULL;
		return 0;
	}
	f->pc = f->sf->reg_window.ret_pc;

	return 1;
}

void reg_window_print(struct reg_window* rw) {
    printk("%%L: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
                    rw->locals[0], rw->locals[1], rw->locals[2], rw->locals[3],
                    rw->locals[4], rw->locals[5], rw->locals[6], rw->locals[7]);
    printk("%%I: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
                    rw->ins[0], rw->ins[1], rw->ins[2], rw->ins[3],
                    rw->ins[4], rw->ins[5], (uint32_t) rw->fp, (uint32_t) rw->ret_pc);
}

void stackframe_print(struct stackframe* f) {
	printk("frame_address = 0x%p, return_address = 0x%p  \n", f->fp, f->pc);
	if (f->sf != NULL) {
        reg_window_print(&f->sf->reg_window);
	}
    printk("\n");
}

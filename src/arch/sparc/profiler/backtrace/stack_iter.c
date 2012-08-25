/**
 * @file
 * @brief TODO: brief
 *
 * @date 24 Aug, 2012
 * @author Bulychev Anton
 */

#include <stdio.h>
#include "stack_iter.h"

#define RETPC_OFFSET 8

void stack_iter_current(stack_iter_t *f) {
	*f = __builtin_frame_address(0);
}

// TODO: May be add type of previous: through interruption or not?
int stack_iter_next(stack_iter_t *f) {
	void *fp = (*f)->reg_window.fp;
	*f = fp;

	if (fp == NULL) {
		return 0;
	} else {
		return 1;
	}
}

static void reg_window_print(struct reg_window *rw) {
    printk("%%L: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
                    rw->locals[0], rw->locals[1], rw->locals[2], rw->locals[3],
                    rw->locals[4], rw->locals[5], rw->locals[6], rw->locals[7]);
    printk("%%I: %08x %08x  %08x %08x  %08x %08x  %08x %08x\n",
                    rw->ins[0], rw->ins[1], rw->ins[2], rw->ins[3],
                    rw->ins[4], rw->ins[5], (uint32_t) rw->fp, (uint32_t) rw->ret_pc);
}

void stack_iter_print(stack_iter_t *f) {
	struct stack_frame *sf = *f;
	struct reg_window *rw = &sf->reg_window;

	// TODO: RETPC_OFFSET

    printk("frame_address = 0x%08x, return_address = 0x%08x  \n",
			(uint32_t) rw->fp, (uint32_t) rw->ret_pc);

	reg_window_print(rw);

    printk("\n");
}

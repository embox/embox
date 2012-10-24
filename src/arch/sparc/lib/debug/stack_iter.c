/**
 * @file
 * @brief TODO: brief
 *
 * @date 24 Aug, 2012
 * @author Bulychev Anton
 */

#include "stack_iter.h"

#define RETPC_OFFSET 8

void stack_iter_current(stack_iter_t *f) {
	winflush();   /* Doesn't work without it */
	*f = __builtin_frame_address(0);
}

int stack_iter_next(stack_iter_t *f) {
	void *fp = (*f)->reg_window.fp;
	*f = fp;

	if ((*f)->reg_window.fp == NULL) {
		return 0;
	} else {
		return 1;
	}
}

void* stack_iter_get_fp(stack_iter_t *f) {
	return (*f)->reg_window.fp;
}

void* stack_iter_get_retpc(stack_iter_t *f) {
	return (*f)->reg_window.ret_pc + RETPC_OFFSET;
}

/**
 * @file
 * @brief TODO: brief
 *
 * @date 24 Aug, 2012
 * @author Bulychev Anton
 */

#include <stddef.h>

#include <hal/context.h>
#include <asm/ptrace.h>

#include "stack_iter.h"

#define RETPC_OFFSET 8

void stack_iter_context(stack_iter_t *f, struct context *ctx) {
	// XXX stub -- Eldar
	(*f) = (void*)ctx->kregs.ksp;
}

void stack_iter_current(stack_iter_t *f) {
	winflush();   /* Doesn't work without it */
	*f = __builtin_frame_address(0);
}

int stack_iter_next(stack_iter_t *f) {
	void *fp = (*f)->reg_window.fp;
	*f = fp;

	if (((*f)->reg_window.fp == NULL) || ((uint32_t)(*f)->reg_window.fp & 0x7)) {
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

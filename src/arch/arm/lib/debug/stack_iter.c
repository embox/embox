/**
 * @file stack_iter.c
 * @brief 
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-29
 *
 * @note Remember to add -mapcs-frame option to CFLAGS as 
 * different frame formats require different offets
 */

#include <stddef.h>

#include <hal/context.h>
#include <util/lang.h>
#include <util/math.h>

#include "stack_iter.h"

int stack_iter_next(stack_iter_t *f) {
	f->fp = (void *) *((int *) (f->fp - 0xc));
	f->lr = (void *) *((int *) (f->fp - 0x4));

	return (void *) *((int *) (f->fp - 0xc)) != 0;
}

void stack_iter_context(stack_iter_t *f, struct context *ctx) {
	f->fp = (void*) ctx->system_r[11];	/* R11 is frame pointer */
	f->lr = (void*) ctx->lr;		/* R14 is link register */
}

void stack_iter_current(stack_iter_t *f) {
	__asm__ __volatile__ (
		"mov %[fp], FP\n\t"
		"mov %[lr], LR\n\t"
		: [fp]"=r"(f->fp),
		  [lr]"=r"(f->lr) : :
	);
	
	/* We can't just take those registers
	 * because caller requests _his_ context,
	 * not the context of stack_iter_current(),
	 * so we have to make one unwind operation
	 * by hand */

	stack_iter_next(f);
}

void *stack_iter_get_retpc(stack_iter_t *f) {
	/* LR stores the actual address we should return to. */
	return f->lr;
}

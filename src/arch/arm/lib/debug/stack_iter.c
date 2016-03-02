/**
 * @file stack_iter.c
 * @brief 
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-02-29
 */

#include <stddef.h>

#include <hal/context.h>
#include <util/lang.h>
#include <util/math.h>

#include "stack_iter.h"

void stack_iter_context(stack_iter_t *f, struct context *ctx) {
}

void stack_iter_current(stack_iter_t *f) {
	__asm__ __volatile__ (
		"mov %[fp], FP\n\t"
		"mov %[lr], LR\n\t"
		"mov %[pc], PC\n\t"
		: [fp]"=r"(f->fp),
		  [lr]"=r"(f->lr),
		  [pc]"=r"(f->pc) : :
	);
	
	/* We can't just take those registers
	 * because caller requests _his_ context,
	 * not the context of stack_iter_current(),
	 * so we have to make one unwind operation
	 * by hand */

	f->fp = (void*) *((int*)f->fp - 3);
	f->pc = (void*) *((int*)f->fp);
	f->lr = (void*) *((int*)f->fp - 1);
	f->fp = (void*) *((int*)f->fp - 3);
}

int stack_iter_next(stack_iter_t *f) {
	return 0;
}

void *stack_iter_get_fp(stack_iter_t *f) {
	return NULL;
}

void *stack_iter_get_retpc(stack_iter_t *f) {
	return NULL;
}

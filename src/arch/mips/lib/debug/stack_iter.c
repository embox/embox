/**
 * @brief
 * @author Nastya Nizharadze
 * @date 30.07.2019
 */

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include <hal/context.h>
#include <util/lang.h>
#include <util/math.h>

#include "debug/symbol.h"
#include "stack_iter.h"

/* Mips stack:
 *
 * high address ^
 * 
 *  -------- frame begining
 * |        | ---		<- new fp + offset
 * |   ra   |  ^		<- new fp + offset - 4
 * | old fp |  |		<- new fp + offset - 8
 * |   ...  |  | offset
 * |        |  |
 * |        |  v
 * |        | ---		<- new fp
 *  -------- frame ending
 * 
 *  */

static int get_offset(void *ra) {
	uint32_t opcode;
	int16_t offset;
	const struct symbol *a;

	/* Find an address of first function's instruction */
	a = symbol_lookup(ra);
	if (a == NULL) {
		return 0;
	}
	/* Find an operation code. Operation is always addiu sp,sp,x
 * where x is an offset. Get 4 last digits to find x*/
	opcode = *((uint32_t *)(a->addr));
	offset = opcode & 0xffff;
	assert(offset < 0);
	return -offset;
}

int stack_iter_next(stack_iter_t *f) {
	/* To get an old fp we need to cast the contents of this  |
 * block of memory to the void pointer type               V */
	f->fp = (void *)*((uintptr_t *)(f->fp + get_offset(f->ra) - 8));
	f->ra = (void *)*((uintptr_t *)(f->fp - 4));

	if (!(get_offset(f->ra))) {
		return 0;
	}
	return 1;
}

void stack_iter_context(stack_iter_t *f, struct context *ctx) {
	/* s8 is a frame pointer */
	f->fp = (void *)ctx->s[8];
	f->ra = (void *)ctx->ra;
}

void stack_iter_current(stack_iter_t *f) {
	void *pc = stack_iter_current;

	__asm__ __volatile__("move %[fp], $fp\n\t"
	                     "move %[ra], $ra\n\t"
	                     : [fp] "=r"(f->fp), [ra] "=r"(f->ra)
	                     :
	                     :);
	/**
	 * fp points to the current frame, but ra points to the previous.
	 * So we need to add offset to our fp so that both of pointers 
	 * belongs to the same frame
	*/
	f->fp = (void *)*((uintptr_t *)(f->fp + get_offset(pc) - 8));
	stack_iter_next(f);
}

void *stack_iter_get_retpc(stack_iter_t *f) {
	return f->ra;
}

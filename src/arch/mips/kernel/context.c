/**
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */

#include <stdint.h>
#include <hal/context.h>
#include <asm/mipsregs.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	/* Set kernel mode stack pointer */
	/* extra substract to allow full desceding stack. Just in case */
	ctx->sp = (uint32_t) sp - sizeof(struct context) - 4;
	ctx->ra = (uint32_t) routine_fn;
	ctx->c0_stat = 0;
}

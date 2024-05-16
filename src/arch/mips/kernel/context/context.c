/**
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */

#include <stdint.h>

#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <hal/context.h>

void context_init(struct context *ctx, unsigned int flags,
    void (*routine_fn)(void), void *sp) {
	/* Set kernel mode stack pointer */
	/* extra substract to allow full desceding stack. Just in case */
	ctx->sp = (unsigned long)sp - sizeof(struct context) - sizeof(long);
	ctx->ra = (unsigned long)routine_fn;
	ctx->c0_stat = 0;
}

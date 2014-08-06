/**
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */
#include <stdint.h>
#include <hal/context.h>


void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	/* Set kernel mode stack pointer */
	ctx->sp = (uint32_t) sp;
	ctx->ra = (uint32_t) routine_fn;
}

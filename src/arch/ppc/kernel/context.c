/**
 * @file
 *
 * @date 16.11.12
 * @author Ilia Vaprol
 */

#include <hal/context.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	ctx->sp = (uint32_t) sp - 8;
	ctx->lr = (uint32_t) routine_fn;
}

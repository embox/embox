/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    22.12.2013
 */

#include <assert.h>
#include <string.h>
#include <kernel/host.h>
#include <kernel/thread.h>

#include <hal/context.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));
	host_getcontext(ctx->ucp);
	host_stackcontext(ctx->ucp, sp, 0);
	host_makecontext(ctx->ucp, routine_fn, 0);
}

void context_switch(struct context *prev_ctx, struct context *next_ctx) {
	host_swapcontext(prev_ctx->ucp, next_ctx->ucp);
}

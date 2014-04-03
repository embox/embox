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

void context_init(struct context *ctx, bool privileged) {
	memset(ctx, 0, sizeof(*ctx));
	host_getcontext(ctx->ucp);
	ctx->initing = 1;
}

void context_set_entry(struct context *ctx, void(*pc)(void)) {

	assert(ctx->initing == 1);
	ctx->pc = pc;
}

void context_set_stack(struct context *ctx, void *sp) {

	assert(ctx->initing == 1);
	assert(ctx->pc);

	host_stackcontext(ctx->ucp, sp, 0);

	ctx->initing = 0;
	host_makecontext(ctx->ucp, ctx->pc, 0);
}

void context_switch(struct context *prev_ctx, struct context *next_ctx) {

	host_swapcontext(prev_ctx->ucp, next_ctx->ucp);
}

/*
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */
#include <hal/context.h>


void context_init(struct context *ctx, bool privileged) {
}

void context_set_stack(struct context *ctx, void *sp) {
	/* Set kernel mode stack pointer */
	ctx->sp = (uint32_t) sp;
}

void context_set_entry(struct context *ctx, void(*pc)(void)) {
	ctx->ra = (uint32_t) pc;
}

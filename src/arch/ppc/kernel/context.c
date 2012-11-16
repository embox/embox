/**
 * @file
 *
 * @date 16.11.12
 * @author Ilia Vaprol
 */

#include <hal/context.h>


void context_init(struct context *ctx, bool privileged) {
}

void context_set_stack(struct context *ctx, void *sp) {
	ctx->sp = (uint32_t)sp;
}

void context_set_entry(struct context *ctx, void (*pc)(void)) {
	ctx->lr = (uint32_t)pc;
}

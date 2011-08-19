/**
 * @file
 *
 * @brief
 *
 * @date 29.07.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <assert.h>

#include <hal/context.h>
#include <asm/msr.h>

void context_init(struct context *ctx, bool privileged) {
	ctx->msr = msr_get_value();
}

void context_set_stack(struct context *ctx, void *stack_addr) {
	ctx->r1 = (uint32_t)stack_addr;
}

void context_set_entry(struct context *ctx, void (*pc)(void)) {
	ctx->r15 = (uint32_t) pc - 8;
}

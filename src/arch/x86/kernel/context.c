/**
 * @file
 *
 * @date 1.12.10
 * @author Nikolay Korotky
 */
#include <types.h>
#include <hal/context.h>

void context_init(struct context *ctx, bool privileged) {
}

void context_set_stack(struct context *ctx, void *sp) {
	/* Set kernel mode stack pointer */
	ctx->esp = (uint32_t) sp - 0x40 - sizeof(uint32_t);
}

void context_set_entry(struct context *ctx, void(*pc)(void)) {
	ctx->eip = (uint32_t) pc;
}


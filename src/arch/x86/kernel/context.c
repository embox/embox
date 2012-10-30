/**
 * @file
 *
 * @date 1.12.10
 * @author Nikolay Korotky
 */

#include <types.h>
#include <hal/context.h>

#include <string.h>

void context_init(struct context *ctx, bool privileged) {
}

void context_set_stack(struct context *ctx, void *sp) {
	/* Set kernel mode stack pointer */
	ctx->esp = (uint32_t) sp - 0x40 - sizeof(uint32_t);
}

void context_set_entry(struct context *ctx, void(*pc)(void)) {
	ctx->eip = (uint32_t) pc;
}

void context_enter_frame(struct context *ctx, void *pc) {
	void *stack = (uint32_t *) ctx->esp;

	stack -= sizeof(uint32_t);
	*(uint32_t *) stack = ctx->eip;

	ctx->esp = ((uint32_t) stack);
	context_set_entry(ctx, pc);
}

void context_push_stack(struct context *ctx, void *arg, size_t n) {
	ctx->esp -= n;

	memcpy((void *) ctx->esp, arg, n);

}

void *context_pop_stack(struct context *ctx, size_t n) {
	ctx->esp += n;
	return (void *)ctx->esp;
}

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

void context_set_entry(struct context *ctx, void(*pc)(int), int arg) {
	uint32_t *argp;

	ctx->eip = (uint32_t) pc;

	/* Kernel mode argument */
	argp = (uint32_t *) (ctx->esp + 2 * sizeof(uint32_t));
	*argp = (uint32_t) arg;
}


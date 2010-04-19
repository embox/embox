/**
 * @file
 * @brief TODO
 *
 * @date 13.04.2010
 * @author Eldar Abusalimov
 */

// XXX this is the very very temporal solution. -- Eldar

#include <types.h>
#include <assert.h>

#include <hal/context.h>
#include <asm/psr.h>

void context_init(struct context *ctx, bool privileged) {
	// TODO initial context state... -- Eldar
//	ctx->regs.psr = PSR_S | PSR_ET;
	if (privileged) {
		ctx->regs.psr |= PSR_PS;
	}
}

void context_set_stack(struct context *ctx, void *sp) {
	ctx->regs.ins[6] = (uint32_t) sp;
}

struct context *current_ctx; // XXX I'll fix it soon. -- Eldar
void context_set_entry(struct context *ctx, void (*pc)(int), int arg) {
	if (!ctx) {
		ctx = current_ctx;
	}
	assert(ctx != NULL);
	ctx->regs.ins[7] = (uint32_t) pc - 8;
	ctx->regs.ins[0] = (uint32_t) arg;
}

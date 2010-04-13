/**
 * @file
 * @brief TODO
 *
 * @date 13.04.2010
 * @author Eldar Abusalimov
 */

// XXX this is the very very temporal solution. -- Eldar

#include <types.h>

#include <hal/context.h>
#include <asm/psr.h>

void context_init(struct context *ctx, bool privileged) {
	// TODO initial context state... -- Eldar
	if (privileged) {
		ctx->regs.psr |= PSR_PS;
	}
}

void context_set_stack(struct context *ctx, void *sp) {
	ctx->regs.ins[6] = (uint32_t) sp;
}

struct context *current_ctx; // XXX I'll fix it soon. -- Eldar
void context_set_entry(struct context *ctx, void *pc) {
	if (!ctx) {
		ctx = current_ctx;
	}
	ctx->regs.pc = (uint32_t) pc;
	ctx->regs.npc = (uint32_t) pc + 4;
}

// TODO locore context switch. -- Eldar
// and do not forget to flush all windows.
void context_switch(struct context *prev, struct context *next);

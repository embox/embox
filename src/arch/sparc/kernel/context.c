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
#include <asm/wim.h>
#include <asm/stack.h>

void context_init(struct context *ctx, bool privileged) {
	// TODO initial context state... -- Eldar
	ctx->kregs.psr = PSR_S | PSR_ET;
	if (privileged) {
		ctx->kregs.psr |= PSR_PS;
	}
	ctx->kregs.wim = WIM_INIT;
}

void context_set_stack(struct context *ctx, void *stack_addr) {
	uint32_t sp = (uint32_t) stack_addr;
	sp &= ~0x7;
	ctx->kregs.ksp = sp - STACK_FRAME_SZ;
}

struct context *current_ctx; // XXX I'll fix it soon. -- Eldar
void context_set_entry(struct context *ctx, void (*pc)(void)) {
	if (!ctx) {
		ctx = current_ctx;
	}
	assert(ctx != NULL);
//	struct stack_frame *frame = (struct stack_frame *) ctx->kregs.sp;
	ctx->kregs.ret = (uint32_t) pc - 8;
//	frame->reg_window.ins[0] = arg;
//	frame->reg_window.ins[1] = 0xdeadbeef;
}


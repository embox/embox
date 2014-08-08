/**
 * @file
 * @brief TODO
 *
 * @date 13.04.10
 * @author Eldar Abusalimov
 */

// XXX this is the very very temporal solution. -- Eldar

#include <stdint.h>
#include <assert.h>
#include <stddef.h>

#include <hal/context.h>
#include <asm/psr.h>
#include <asm/wim.h>
#include <asm/stack.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	assert(ctx != NULL);
	// TODO initial context state... -- Eldar
	ctx->kregs.psr = PSR_PIL | PSR_S | PSR_ET;
	if (flags & CONTEXT_PRIVELEGED) {
		ctx->kregs.psr |= PSR_PS;
	}
	ctx->kregs.wim = WIM_INIT;

	ctx->kregs.ksp = ((uint32_t) sp & ~0x7) - STACK_FRAME_SZ;

//	struct stack_frame *frame = (struct stack_frame *) ctx->kregs.sp;
	ctx->kregs.ret = (uint32_t) routine_fn - 8;
//	frame->reg_window.ins[0] = arg;
//	frame->reg_window.ins[1] = 0xdeadbeef;
}

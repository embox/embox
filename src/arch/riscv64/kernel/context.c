/**
 * @file
 *
 * @brief
 *
 * @date 23.06.2020
 * @author sksat
 */

#include <stdint.h>
#include <hal/context.h>
#include <hal/ipl.h>
#include <asm/regs.h>
#include <string.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));

	ctx->sp = (uint64_t) sp;
	ctx->ra = (uint64_t) routine_fn;
	ctx->mstatus = read_csr(mstatus);

	if (flags & CONTEXT_IRQDISABLE) {
		ctx->mstatus &= ~(MSTATUS_MIE);
	}
}

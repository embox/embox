/**
 * @file
 *
 * @brief
 *
 * @date 21.01.2020
 * @author Nastya Nizharadze
 */

#include <stdint.h>
#include <hal/context.h>
#include <hal/ipl.h>
#include <asm/regs.h>
#include <string.h>

void context_init(struct context *ctx, unsigned int flags,
		void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));

	ctx->sp = (unsigned long) sp;
	ctx->ra = (unsigned long) routine_fn;
	ctx->mstatus = read_csr(STATUS_REG);

	if (flags & CONTEXT_IRQDISABLE) {
		ctx->mstatus &= ~(STATUS(IE));
	}
}

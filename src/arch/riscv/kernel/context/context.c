/**
 * @file
 *
 * @brief
 *
 * @date 21.01.2020
 * @author Nastya Nizharadze
 */

#include <stdint.h>
#include <string.h>

#include <asm/asm.h>
#include <asm/regs.h>
#include <hal/context.h>
#include <hal/ipl.h>

void context_init(struct context *ctx, unsigned int flags,
    void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));

	ctx->x.sp = (unsigned long)sp;
	ctx->x.ra = (unsigned long)routine_fn;
	ctx->status = read_csr(STATUS_REG);

#if REG_SIZE_F > 0
		ctx->status &= ~(1UL << 14);
		ctx->status |= MSTATUS_FS;
#endif

	if (flags & CONTEXT_IRQDISABLE) {
		ctx->status &= ~(STATUS(IE));
	}
}

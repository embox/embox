/**
 * @file
 * @brief
 *
 * @date 21.01.2020
 * @author Nastya Nizharadze
 */

#include <stdint.h>
#include <string.h>

#include <asm/asm.h>
#include <asm/csr.h>
#include <hal/context.h>
#include <hal/ipl.h>

void context_init(struct context *ctx, unsigned int flags,
    void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));

	ctx->x.sp = (unsigned long)sp;
	ctx->x.ra = (unsigned long)routine_fn;
	ctx->status = csr_read(CSR_STATUS);

#if REG_SIZE_F > 0
	ctx->status &= ~CSR_STATUS_FS_DIRTY;
	ctx->status |= CSR_STATUS_FS_INIT;
#endif

	if (flags & CONTEXT_IRQDISABLE) {
		ctx->status &= ~(CSR_STATUS_IE);
	}
}

/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.10.2012
 */

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <arm/fpu.h>
#include <hal/context.h>
#include <hal/reg.h>

/* In the RVCT v2.0 and above, all generated code and C library code
 * will maintain eight-byte stack alignment on external interfaces. */
#define ARM_SP_ALIGNMENT 8

void context_init(struct context *ctx, unsigned int flags,
    void (*routine_fn)(void), void *sp) {
	memset(ctx, 0, sizeof(*ctx));

	ctx->ptregs.pc = (uint32_t)routine_fn;
	ctx->ptregs.sp = (uint32_t)sp;
	ctx->ptregs.cpsr = PSR_M_SYS;

	assertf(((uint32_t)sp % ARM_SP_ALIGNMENT) == 0,
	    "Stack pointer is not aligned to 8 bytes.\n"
	    "Firstly please make sure the thread stack size is aligned to 8 bytes");

	if (flags & CONTEXT_IRQDISABLE) {
		ctx->ptregs.cpsr |= PSR_I | PSR_F;
	}

	arm_fpu_context_init(&ctx->fpu_context);
}

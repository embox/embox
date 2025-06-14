/**
 * @file
 *
 * @brief
 *
 * @date 21.01.2020
 * @author Nastya Nizharadze
 */

#include <asm/asm.h>
#include <asm/regs.h>
#include <hal/context.h>

.section .text
.global context_switch

.align 2
context_switch:
	/* Save current context */
	PTR_S   sp, CTX_SP(a0)
	PTR_S   ra, CTX_RA(a0)
	PTR_S   s0, CTX_S0(a0)
	PTR_S   s1, CTX_S1(a0)
	PTR_S   s2, CTX_S2(a0)
	PTR_S   s3, CTX_S3(a0)
	PTR_S   s4, CTX_S4(a0)
	PTR_S   s5, CTX_S5(a0)
	PTR_S   s6, CTX_S6(a0)
	PTR_S   s7, CTX_S7(a0)
	PTR_S   s8, CTX_S8(a0)
	PTR_S   s9, CTX_S9(a0)
	PTR_S   s10, CTX_S10(a0)
	PTR_S   s11, CTX_S11(a0)
	csrr    t6, STATUS_REG
	PTR_S   t6, CTX_STATUS(a0)

	/* Load new context */
	PTR_L   sp, CTX_SP(a1)
	PTR_L   ra, CTX_RA(a1)
	PTR_L   s0, CTX_S0(a1)
	PTR_L   s1, CTX_S1(a1)
	PTR_L   s2, CTX_S2(a1)
	PTR_L   s3, CTX_S3(a1)
	PTR_L   s4, CTX_S4(a1)
	PTR_L   s5, CTX_S5(a1)
	PTR_L   s6, CTX_S6(a1)
	PTR_L   s7, CTX_S7(a1)
	PTR_L   s8, CTX_S8(a1)
	PTR_L   s9, CTX_S9(a1)
	PTR_L   s10, CTX_S10(a1)
	PTR_L   s11, CTX_S11(a1)
	PTR_L   t6, CTX_STATUS(a1)
	csrw    STATUS_REG, t6
	ret

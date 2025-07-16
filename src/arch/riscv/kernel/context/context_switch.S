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
	REG_S   sp, CTX_SP(a0)
	REG_S   ra, CTX_RA(a0)
	REG_S   s0, CTX_S0(a0)
	REG_S   s1, CTX_S1(a0)
	REG_S   s2, CTX_S2(a0)
	REG_S   s3, CTX_S3(a0)
	REG_S   s4, CTX_S4(a0)
	REG_S   s5, CTX_S5(a0)
	REG_S   s6, CTX_S6(a0)
	REG_S   s7, CTX_S7(a0)
	REG_S   s8, CTX_S8(a0)
	REG_S   s9, CTX_S9(a0)
	REG_S   s10, CTX_S10(a0)
	REG_S   s11, CTX_S11(a0)
	csrr    t6, STATUS_REG
	REG_S   t6, CTX_STATUS(a0)

	/* Load new context */
	REG_L   sp, CTX_SP(a1)
	REG_L   ra, CTX_RA(a1)
	REG_L   s0, CTX_S0(a1)
	REG_L   s1, CTX_S1(a1)
	REG_L   s2, CTX_S2(a1)
	REG_L   s3, CTX_S3(a1)
	REG_L   s4, CTX_S4(a1)
	REG_L   s5, CTX_S5(a1)
	REG_L   s6, CTX_S6(a1)
	REG_L   s7, CTX_S7(a1)
	REG_L   s8, CTX_S8(a1)
	REG_L   s9, CTX_S9(a1)
	REG_L   s10, CTX_S10(a1)
	REG_L   s11, CTX_S11(a1)
	REG_L   t6, CTX_STATUS(a1)
	csrw    STATUS_REG, t6
	ret

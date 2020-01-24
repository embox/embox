/**
 * @file
 *
 * @brief
 *
 * @date 21.01.2020
 * @author Nastya Nizharadze
 */

#include "context.h"

.align 4
.global context_switch
context_switch:
	addi    sp, sp, -CTX_SIZE
	sw      sp, CTX_SP(a0)
	sw      ra, CTX_RA(a0)
	sw      s0, CTX_S0(a0)
	sw      s1, CTX_S1(a0)
	sw      s2, CTX_S2(a0)
	sw      s3, CTX_S3(a0)
	sw      s4, CTX_S4(a0)
	sw      s5, CTX_S5(a0)
	sw      s6, CTX_S6(a0)
	sw      s7, CTX_S7(a0)
	sw      s8, CTX_S8(a0)
	sw      s9, CTX_S9(a0)
	sw      s10, CTX_S10(a0)
	sw      s11, CTX_S11(a0)
	csrr    t6, mstatus
	sw      t6, CTX_MSTATUS(a0)
	
	lw      sp, CTX_SP(a1)

	lw      ra, CTX_RA(a1)
	lw      s0, CTX_S0(a1)
	lw      s1, CTX_S1(a1)
	lw      s2, CTX_S2(a1)
	lw      s3, CTX_S3(a1)
	lw      s4, CTX_S4(a1)
	lw      s5, CTX_S5(a1)
	lw      s6, CTX_S6(a1)
	lw      s7, CTX_S7(a1)
	lw      s8, CTX_S8(a1)
	lw      s9, CTX_S9(a1)
	lw      s10, CTX_S10(a1)
	lw      s11, CTX_S11(a1)
	lw      t6, CTX_MSTATUS(a1)
	csrw    mstatus, t6
	addi    sp, sp, CTX_SIZE

	ret

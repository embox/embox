/**
 * @file
 *
 * @brief
 *
 * @date 23.06.2020
 * @author sksat
 */

#include "context.h"

.align 8
.global context_switch
context_switch:
	addi    sp, sp, -CTX_SIZE
	sd      sp, CTX_SP(a0)
	sd      ra, CTX_RA(a0)
	sd      s0, CTX_S0(a0)
	sd      s1, CTX_S1(a0)
	sd      s2, CTX_S2(a0)
	sd      s3, CTX_S3(a0)
	sd      s4, CTX_S4(a0)
	sd      s5, CTX_S5(a0)
	sd      s6, CTX_S6(a0)
	sd      s7, CTX_S7(a0)
	sd      s8, CTX_S8(a0)
	sd      s9, CTX_S9(a0)
	sd      s10, CTX_S10(a0)
	sd      s11, CTX_S11(a0)
	csrr    t6, mstatus
	sd      t6, CTX_MSTATUS(a0)
	
	ld      sp, CTX_SP(a1)

	ld      ra, CTX_RA(a1)
	ld      s0, CTX_S0(a1)
	ld      s1, CTX_S1(a1)
	ld      s2, CTX_S2(a1)
	ld      s3, CTX_S3(a1)
	ld      s4, CTX_S4(a1)
	ld      s5, CTX_S5(a1)
	ld      s6, CTX_S6(a1)
	ld      s7, CTX_S7(a1)
	ld      s8, CTX_S8(a1)
	ld      s9, CTX_S9(a1)
	ld      s10, CTX_S10(a1)
	ld      s11, CTX_S11(a1)
	ld      t6, CTX_MSTATUS(a1)
	csrw    mstatus, t6
	addi    sp, sp, CTX_SIZE

	ret

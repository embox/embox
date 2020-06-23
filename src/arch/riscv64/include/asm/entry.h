/**
 * @file
 *
 * @brief
 *
 * @date 23.06.2019
 * @author sksat
 */

#ifndef RISCV64_ENTRY_H_
#define RISCV64_ENTRY_H_

#include <asm/ptrace.h>

#ifndef __ASSEMBLER__

#else /* __ASSEMBLER__ */

	.macro SAVE_ALL

		addi    sp, sp, -PT_SIZE
		sd      t6, PT_R31(sp)
		sd      sp, PT_SP(sp)
		sd      ra, PT_RA(sp)
		sd      gp, PT_GP(sp)
		sd      tp, PT_TP(sp)
		sd      t0, PT_R5(sp)
		sd      t1, PT_R6(sp)
		sd      t2, PT_R7(sp)
		sd      s0, PT_R8(sp)
		sd      s1, PT_R9(sp)
		sd      a0, PT_R10(sp)
		sd      a1, PT_R11(sp)
		sd      a2, PT_R12(sp)
		sd      a3, PT_R13(sp)
		sd      a4, PT_R14(sp)
		sd      a5, PT_R15(sp)
		sd      a6, PT_R16(sp)
		sd      a7, PT_R17(sp)
		sd      s2, PT_R18(sp)
		sd      s3, PT_R19(sp)
		sd      s4, PT_R20(sp)
		sd      s5, PT_R21(sp)
		sd      s6, PT_R22(sp)
		sd      s7, PT_R23(sp)
		sd      s8, PT_R24(sp)
		sd      s9, PT_R25(sp)
		sd      s10, PT_R26(sp)
		sd      s11, PT_R27(sp)
		sd      t3, PT_R28(sp)
		sd      t4, PT_R29(sp)
		sd      t5, PT_R30(sp)

		csrr    t6, mstatus
		sd      t6, PT_MSTATUS(sp)
		csrr    t6, mepc
		sd      t6, PT_PC(sp)

	.endm

	.macro RESTORE_ALL

		ld      ra, PT_RA(sp)
		ld      gp, PT_GP(sp)
		ld      tp, PT_TP(sp)
		ld      t0, PT_R5(sp)
		ld      t1, PT_R6(sp)
		ld      t2, PT_R7(sp)
		ld      s0, PT_R8(sp)
		ld      s1, PT_R9(sp)
		ld      a0, PT_R10(sp)
		ld      a1, PT_R11(sp)
		ld      a2, PT_R12(sp)
		ld      a3, PT_R13(sp)
		ld      a4, PT_R14(sp)
		ld      a5, PT_R15(sp)
		ld      a6, PT_R16(sp)
		ld      a7, PT_R17(sp)
		ld      s2, PT_R18(sp)
		ld      s3, PT_R19(sp)
		ld      s4, PT_R20(sp)
		ld      s5, PT_R21(sp)
		ld      s6, PT_R22(sp)
		ld      s7, PT_R23(sp)
		ld      s8, PT_R24(sp)
		ld      s9, PT_R25(sp)
		ld      s10, PT_R26(sp)
		ld      s11, PT_R27(sp)
		ld      t3, PT_R28(sp)
		ld      t4, PT_R29(sp)
		ld      t5, PT_R30(sp)

		ld      t6, PT_MSTATUS(sp)
		csrw    mstatus, t6

		ld      t6, PT_PC(sp)
		csrw    mepc, t6

		ld      t6, PT_R31(sp)

		ld      sp, PT_SP(sp)
		addi    sp, sp, PT_SIZE

	.endm

#endif /* __ASSEMBLER __ */

#endif /* RISCV64_ENTRY_H_ */

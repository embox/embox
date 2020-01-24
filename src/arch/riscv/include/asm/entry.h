/**
 * @file
 *
 * @brief
 *
 * @date 26.11.2019
 * @author Nastya Nizharadze
 */

#ifndef RISCV_ENTRY_H_
#define RISCV_ENTRY_H_

#include <asm/ptrace.h>

#ifndef __ASSEMBLER__

#else /* __ASSEMBLER__ */

	.macro SAVE_ALL

		addi    sp, sp, -PT_SIZE
		sw      t6, PT_R31(sp)
		sw      sp, PT_SP(sp)
		sw      ra, PT_RA(sp)
		sw      gp, PT_GP(sp)
		sw      tp, PT_TP(sp)
		sw      t0, PT_R5(sp)
		sw      t1, PT_R6(sp)
		sw      t2, PT_R7(sp)
		sw      s0, PT_R8(sp)
		sw      s1, PT_R9(sp)
		sw      a0, PT_R10(sp)
		sw      a1, PT_R11(sp)
		sw      a2, PT_R12(sp)
		sw      a3, PT_R13(sp)
		sw      a4, PT_R14(sp)
		sw      a5, PT_R15(sp)
		sw      a6, PT_R16(sp)
		sw      a7, PT_R17(sp)
		sw      s2, PT_R18(sp)
		sw      s3, PT_R19(sp)
		sw      s4, PT_R20(sp)
		sw      s5, PT_R21(sp)
		sw      s6, PT_R22(sp)
		sw      s7, PT_R23(sp)
		sw      s8, PT_R24(sp)
		sw      s9, PT_R25(sp)
		sw      s10, PT_R26(sp)
		sw      s11, PT_R27(sp)
		sw      t3, PT_R28(sp)
		sw      t4, PT_R29(sp)
		sw      t5, PT_R30(sp)

		csrr    t6, mstatus
		sw      t6, PT_MSTATUS(sp)
		csrr    t6, mepc
		sw      t6, PT_PC(sp)

	.endm

	.macro RESTORE_ALL

		lw      ra, PT_RA(sp)
		lw      gp, PT_GP(sp)
		lw      tp, PT_TP(sp)
		lw      t0, PT_R5(sp)
		lw      t1, PT_R6(sp)
		lw      t2, PT_R7(sp)
		lw      s0, PT_R8(sp)
		lw      s1, PT_R9(sp)
		lw      a0, PT_R10(sp)
		lw      a1, PT_R11(sp)
		lw      a2, PT_R12(sp)
		lw      a3, PT_R13(sp)
		lw      a4, PT_R14(sp)
		lw      a5, PT_R15(sp)
		lw      a6, PT_R16(sp)
		lw      a7, PT_R17(sp)
		lw      s2, PT_R18(sp)
		lw      s3, PT_R19(sp)
		lw      s4, PT_R20(sp)
		lw      s5, PT_R21(sp)
		lw      s6, PT_R22(sp)
		lw      s7, PT_R23(sp)
		lw      s8, PT_R24(sp)
		lw      s9, PT_R25(sp)
		lw      s10, PT_R26(sp)
		lw      s11, PT_R27(sp)
		lw      t3, PT_R28(sp)
		lw      t4, PT_R29(sp)
		lw      t5, PT_R30(sp)

		lw      t6, PT_MSTATUS(sp)
		csrw    mstatus, t6

		lw      t6, PT_PC(sp)
		csrw    mepc, t6

		lw      t6, PT_R31(sp)

		lw      sp, PT_SP(sp)
		addi    sp, sp, PT_SIZE

	.endm

#endif /* __ASSEMBLER __ */

#endif /* RISCV_ENTRY_H_ */
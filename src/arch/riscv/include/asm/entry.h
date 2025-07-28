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

#include <asm/asm.h>
#include <asm/csr.h>
#include <asm/ptrace.h>

#ifdef __ASSEMBLER__

.macro SAVE_ALL
	addi    sp, sp, -PT_SIZE
	REG_S   t6, PT_R31(sp)
	addi    t6, sp, PT_SIZE

	REG_S   ra, PT_RA(sp)
	REG_S   t6, PT_SP(sp)
	REG_S   gp, PT_GP(sp)
	REG_S   tp, PT_TP(sp)
	REG_S   t0, PT_R5(sp)
	REG_S   t1, PT_R6(sp)
	REG_S   t2, PT_R7(sp)
	REG_S   s0, PT_R8(sp)
	REG_S   s1, PT_R9(sp)
	REG_S   a0, PT_R10(sp)
	REG_S   a1, PT_R11(sp)
	REG_S   a2, PT_R12(sp)
	REG_S   a3, PT_R13(sp)
	REG_S   a4, PT_R14(sp)
	REG_S   a5, PT_R15(sp)
	REG_S   a6, PT_R16(sp)
	REG_S   a7, PT_R17(sp)
	REG_S   s2, PT_R18(sp)
	REG_S   s3, PT_R19(sp)
	REG_S   s4, PT_R20(sp)
	REG_S   s5, PT_R21(sp)
	REG_S   s6, PT_R22(sp)
	REG_S   s7, PT_R23(sp)
	REG_S   s8, PT_R24(sp)
	REG_S   s9, PT_R25(sp)
	REG_S   s10, PT_R26(sp)
	REG_S   s11, PT_R27(sp)
	REG_S   t3, PT_R28(sp)
	REG_S   t4, PT_R29(sp)
	REG_S   t5, PT_R30(sp)

	csrr    t6, STATUS_REG
	REG_S   t6, PT_MSTATUS(sp)
	csrr    t6, EPC_REG
	REG_S   t6, PT_PC(sp)
.endm

.macro RESTORE_ALL
	REG_L   ra, PT_RA(sp)
	REG_L   gp, PT_GP(sp)
	REG_L   tp, PT_TP(sp)
	REG_L   t0, PT_R5(sp)
	REG_L   t1, PT_R6(sp)
	REG_L   t2, PT_R7(sp)
	REG_L   s0, PT_R8(sp)
	REG_L   s1, PT_R9(sp)
	REG_L   a0, PT_R10(sp)
	REG_L   a1, PT_R11(sp)
	REG_L   a2, PT_R12(sp)
	REG_L   a3, PT_R13(sp)
	REG_L   a4, PT_R14(sp)
	REG_L   a5, PT_R15(sp)
	REG_L   a6, PT_R16(sp)
	REG_L   a7, PT_R17(sp)
	REG_L   s2, PT_R18(sp)
	REG_L   s3, PT_R19(sp)
	REG_L   s4, PT_R20(sp)
	REG_L   s5, PT_R21(sp)
	REG_L   s6, PT_R22(sp)
	REG_L   s7, PT_R23(sp)
	REG_L   s8, PT_R24(sp)
	REG_L   s9, PT_R25(sp)
	REG_L   s10, PT_R26(sp)
	REG_L   s11, PT_R27(sp)
	REG_L   t3, PT_R28(sp)
	REG_L   t4, PT_R29(sp)
	REG_L   t5, PT_R30(sp)

	REG_L   t6, PT_MSTATUS(sp)
	csrw    STATUS_REG, t6

	REG_L   t6, PT_PC(sp)
	csrw    EPC_REG, t6

	REG_L   t6, PT_R31(sp)

	REG_L   sp, PT_SP(sp)
.endm

#endif /* __ASSEMBLER __ */

#endif /* RISCV_ENTRY_H_ */

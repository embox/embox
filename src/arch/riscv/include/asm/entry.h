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
#include <asm/regs.h>
#include <asm/ptrace.h>

#ifdef __ASSEMBLER__

.macro SAVE_ALL
	addi    sp, sp, -PT_SIZE
	PTR_S   t6, PT_R31(sp)
	addi    t6, sp, PT_SIZE

	PTR_S   ra, PT_RA(sp)
	PTR_S   t6, PT_SP(sp)
	PTR_S   gp, PT_GP(sp)
	PTR_S   tp, PT_TP(sp)
	PTR_S   t0, PT_R5(sp)
	PTR_S   t1, PT_R6(sp)
	PTR_S   t2, PT_R7(sp)
	PTR_S   s0, PT_R8(sp)
	PTR_S   s1, PT_R9(sp)
	PTR_S   a0, PT_R10(sp)
	PTR_S   a1, PT_R11(sp)
	PTR_S   a2, PT_R12(sp)
	PTR_S   a3, PT_R13(sp)
	PTR_S   a4, PT_R14(sp)
	PTR_S   a5, PT_R15(sp)
	PTR_S   a6, PT_R16(sp)
	PTR_S   a7, PT_R17(sp)
	PTR_S   s2, PT_R18(sp)
	PTR_S   s3, PT_R19(sp)
	PTR_S   s4, PT_R20(sp)
	PTR_S   s5, PT_R21(sp)
	PTR_S   s6, PT_R22(sp)
	PTR_S   s7, PT_R23(sp)
	PTR_S   s8, PT_R24(sp)
	PTR_S   s9, PT_R25(sp)
	PTR_S   s10, PT_R26(sp)
	PTR_S   s11, PT_R27(sp)
	PTR_S   t3, PT_R28(sp)
	PTR_S   t4, PT_R29(sp)
	PTR_S   t5, PT_R30(sp)

	csrr    t6, STATUS_REG
	PTR_S   t6, PT_MSTATUS(sp)
	csrr    t6, EPC_REG
	PTR_S   t6, PT_PC(sp)
.endm

.macro RESTORE_ALL
	PTR_L   ra, PT_RA(sp)
	PTR_L   gp, PT_GP(sp)
	PTR_L   tp, PT_TP(sp)
	PTR_L   t0, PT_R5(sp)
	PTR_L   t1, PT_R6(sp)
	PTR_L   t2, PT_R7(sp)
	PTR_L   s0, PT_R8(sp)
	PTR_L   s1, PT_R9(sp)
	PTR_L   a0, PT_R10(sp)
	PTR_L   a1, PT_R11(sp)
	PTR_L   a2, PT_R12(sp)
	PTR_L   a3, PT_R13(sp)
	PTR_L   a4, PT_R14(sp)
	PTR_L   a5, PT_R15(sp)
	PTR_L   a6, PT_R16(sp)
	PTR_L   a7, PT_R17(sp)
	PTR_L   s2, PT_R18(sp)
	PTR_L   s3, PT_R19(sp)
	PTR_L   s4, PT_R20(sp)
	PTR_L   s5, PT_R21(sp)
	PTR_L   s6, PT_R22(sp)
	PTR_L   s7, PT_R23(sp)
	PTR_L   s8, PT_R24(sp)
	PTR_L   s9, PT_R25(sp)
	PTR_L   s10, PT_R26(sp)
	PTR_L   s11, PT_R27(sp)
	PTR_L   t3, PT_R28(sp)
	PTR_L   t4, PT_R29(sp)
	PTR_L   t5, PT_R30(sp)

	PTR_L   t6, PT_MSTATUS(sp)
	csrw    STATUS_REG, t6

	PTR_L   t6, PT_PC(sp)
	csrw    EPC_REG, t6

	PTR_L   t6, PT_R31(sp)

	PTR_L   sp, PT_SP(sp)
.endm

#endif /* __ASSEMBLER __ */

#endif /* RISCV_ENTRY_H_ */

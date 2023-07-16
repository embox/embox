/**
 * @file
 *
 * @brief
 *
 * @date 26.11.2019
 * @author Nastya Nizharadze
 */

#ifndef RISCV_PTRACE_H_
#define RISCV_PTRACE_H_

#ifndef __ASSEMBLER__
/* registers description
 * $0 - always zero
 * $1 (ra) return address
 * $2 (sp) stack pointer
 * $3 (gp) global pointer
 * $4 (tp) thread pointer
 * $5..$7 (t0-t2) temporary registers
 * $8 (s0 or fp) saved register 0 or frame pointer
 * $9 (s1) saved register 1
 * $10..$11 (a0-a1) return values or function arguments
 * $12..$17 (a2-a7) function arguments
 * $18..$27 (s2-s11) saved registers
 * $28..$31 (t3-t6) temporary registers
 * pc program counter
 *
 */
typedef struct pt_regs {
	unsigned long ra; /* return address r1 */
	unsigned long sp; /* stack pointer r2 */
	unsigned long gp; /* global pointer r3 */
	unsigned long tp; /* thread pointer r4 */
	unsigned long reg[27];
	unsigned long mstatus;
	unsigned long pc;
} pt_regs_t;

static inline void ptregs_retcode(struct pt_regs *ptregs, int retcode) {
	ptregs->reg[5] = retcode; /* $a0 */
}
#else /* !__ASSEMBLER__ */
#include <asm/asm.h>

/*
 * Offsets of register values in saved context.
 */
#define PT_RA      (SZREG * 0)
#define PT_SP      (SZREG * 1)
#define PT_GP      (SZREG * 2)
#define PT_TP      (SZREG * 3)
#define PT_R5      (SZREG * 4)
#define PT_R6      (SZREG * 5)
#define PT_R7      (SZREG * 6)
#define PT_R8      (SZREG * 7)
#define PT_R9      (SZREG * 8)
#define PT_R10     (SZREG * 9)
#define PT_R11     (SZREG * 10)
#define PT_R12     (SZREG * 11)
#define PT_R13     (SZREG * 12)
#define PT_R14     (SZREG * 13)
#define PT_R15     (SZREG * 14)
#define PT_R16     (SZREG * 15)
#define PT_R17     (SZREG * 16)
#define PT_R18     (SZREG * 17)
#define PT_R19     (SZREG * 18)
#define PT_R20     (SZREG * 19)
#define PT_R21     (SZREG * 20)
#define PT_R22     (SZREG * 21)
#define PT_R23     (SZREG * 22)
#define PT_R24     (SZREG * 23)
#define PT_R25     (SZREG * 24)
#define PT_R26     (SZREG * 25)
#define PT_R27     (SZREG * 26)
#define PT_R28     (SZREG * 27)
#define PT_R29     (SZREG * 28)
#define PT_R30     (SZREG * 29)
#define PT_R31     (SZREG * 30)
#define PT_MSTATUS (SZREG * 31)
#define PT_PC      (SZREG * 32)

/* sizeof(pt_regs) */
#define PT_SIZE    (SZREG * 33)
#endif /* __ASSEMBLER__ */

#endif /* RISCV_PTRACE_H_ */

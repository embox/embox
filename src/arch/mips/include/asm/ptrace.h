/**
 * @file
 * @brief
 *
 * @date 25.07.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_PTRACE_H_
#define MIPS_PTRACE_H_

#include <asm/asm.h>

#ifndef __ASSEMBLER__

/* registers description
 * $0 - always zero
 * $1 ($at) Reserved for the assembler.
 * $2..$3 (v0-v1) hold the integer type function results
 * $4..$7 (a0-a3) first 4 function arguments
 * $8..$15 (t0-t7) temporary registers
 * $16..$23 (s0-s7) Saved registers
 * $24..$25 (t8-t9) Temporary registers
 * $26..27 (k0-k1) reserved for the operating system kernel.
 * $28 (gp) global pointer
 * $29 (sp) stack pointer
 * $30 (fp) frame pointer
 * $31 (ra) return address
 *
 */
typedef struct pt_regs {
	unsigned long reg[25];
	unsigned long gp; /* global pointer r28 */
	unsigned long sp; /* stack pointer r29 */
	unsigned long fp; /* frame pointer r30 */
	unsigned long ra; /* return address r31 */
	unsigned long lo;
	unsigned long hi;
	unsigned long cp0_status;
	unsigned long pc;
} pt_regs_t;

static inline void ptregs_retcode(struct pt_regs *ptregs, int retcode) {
	ptregs->reg[1] = retcode; /* $v0 ($2) */
}

#endif /* !__ASSEMBLER__ */

/*
 * Offsets of register values in saved context.
 */
#define PT_R1     (LONGSIZE * 0)
#define PT_R2     (LONGSIZE * 1)
#define PT_R3     (LONGSIZE * 2)
#define PT_R4     (LONGSIZE * 3)
#define PT_R5     (LONGSIZE * 4)
#define PT_R6     (LONGSIZE * 5)
#define PT_R7     (LONGSIZE * 6)
#define PT_R8     (LONGSIZE * 7)
#define PT_R9     (LONGSIZE * 8)
#define PT_R10    (LONGSIZE * 9)
#define PT_R11    (LONGSIZE * 10)
#define PT_R12    (LONGSIZE * 11)
#define PT_R13    (LONGSIZE * 12)
#define PT_R14    (LONGSIZE * 13)
#define PT_R15    (LONGSIZE * 14)
#define PT_R16    (LONGSIZE * 15)
#define PT_R17    (LONGSIZE * 16)
#define PT_R18    (LONGSIZE * 17)
#define PT_R19    (LONGSIZE * 18)
#define PT_R20    (LONGSIZE * 19)
#define PT_R21    (LONGSIZE * 20)
#define PT_R22    (LONGSIZE * 21)
#define PT_R23    (LONGSIZE * 22)
#define PT_R24    (LONGSIZE * 23)
#define PT_R25    (LONGSIZE * 24)
#define PT_GP     (LONGSIZE * 25)
#define PT_SP     (LONGSIZE * 26)
#define PT_FP     (LONGSIZE * 27)
#define PT_RA     (LONGSIZE * 28)
#define PT_LO     (LONGSIZE * 29)
#define PT_HI     (LONGSIZE * 30)
#define PT_STATUS (LONGSIZE * 31)
#define PT_PC     (LONGSIZE * 32)
/* sizeof(pt_regs) */
#define PT_SIZE   (LONGSIZE * 33)

#endif /* MIPS_PTRACE_H_ */

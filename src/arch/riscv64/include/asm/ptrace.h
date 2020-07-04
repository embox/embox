/**
 * @file
 *
 * @brief
 *
 * @date 23.06.2019
 * @author sksat
 */

#ifndef RISCV64_PTRACE_H_
#define RISCV64_PTRACE_H_

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
	unsigned int ra; /* return address r1 */
	unsigned int sp; /* stack pointer r2 */
	unsigned int gp; /* global pointer r3 */
	unsigned int tp; /* thread pointer r4 */
	unsigned int reg[27];
	unsigned int mstatus;
	unsigned int pc;
} pt_regs_t;

static inline void ptregs_retcode(struct pt_regs *ptregs, int retcode) {
	ptregs->reg[5] = retcode; /* $a0 */
}

#else /* __ASSEMBLER__ */

/*
 * Offsets of register values in saved context.
 */
#define PT_RA      0x00
#define PT_SP      0x08
#define PT_GP      0x10
#define PT_TP      0x18
#define PT_R5      0x20
#define PT_R6      0x28
#define PT_R7      0x30
#define PT_R8      0x38
#define PT_R9      0x40
#define PT_R10     0x48
#define PT_R11     0x50
#define PT_R12     0x58
#define PT_R13     0x60
#define PT_R14     0x68
#define PT_R15     0x70
#define PT_R16     0x78
#define PT_R17     0x80
#define PT_R18     0x88
#define PT_R19     0x90
#define PT_R20     0x98
#define PT_R21     0xA0
#define PT_R22     0xA8
#define PT_R23     0xB0
#define PT_R24     0xB8
#define PT_R25     0xC0
#define PT_R26     0xC8
#define PT_R27     0xD0
#define PT_R28     0xD8
#define PT_R29     0xE0
#define PT_R30     0xE8
#define PT_R31     0xF0

#define PT_MSTATUS 0xF8
#define PT_PC      0x100

/* sizeof(pt_regs) */
#define PT_SIZE    0x108

#endif /* __ASSEMBLER__ */

#endif /* RISCV64_PTRACE_H_ */

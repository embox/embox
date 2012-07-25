/**
 * @file
 *
 * @brief
 *
 * @date 25.07.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_PTRACE_H_
#define MIPS_PTRACE_H_


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
	unsigned int reg[25];
	unsigned int gp; /* global pointer */
	unsigned int sp; /* stack pointer */
	unsigned int fp; /* frame pointer */
	unsigned int ra; /* return address */
	unsigned int lo;
	unsigned int hi;
	unsigned int cp0_status;
	unsigned int pc;
}pt_regs_t;

#else
/*
 * Offsets of register values in saved context.
 */
#define PT_R1     0
#define PT_R2     1
#define PT_R3     2
#define PT_R4     3
#define PT_R5     4
#define PT_R6     5
#define PT_R7     6
#define PT_R8     7
#define PT_R9     8
#define PT_R10    9
#define PT_R11    10
#define PT_R12    11
#define PT_R13    12
#define PT_R14    13
#define PT_R15    14
#define PT_R16    15
#define PT_R17    16
#define PT_R18    17
#define PT_R19    18
#define PT_R20    19
#define PT_R21    20
#define PT_R22    21
#define PT_R23    22
#define PT_R24    23
#define PT_R25    24
#define PT_GP     25
#define PT_SP     26
#define PT_FP     27
#define PT_RA     28
#define PT_LO     29
#define PT_HI     30
#define PT_STATUS 31
#define PT_PC     32

#define PT_WORDS  33
#endif /* __ASSEMBLER__ */

#endif /* MIPS_PTRACE_H_ */

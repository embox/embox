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
	unsigned int gp; /* global pointer r28 */
	unsigned int sp; /* stack pointer r29 */
	unsigned int fp; /* frame pointer r30 */
	unsigned int ra; /* return address 31*/
	unsigned int lo;
	unsigned int hi;
	unsigned int cp0_status;
	unsigned int pc;
}pt_regs_t;

#else /* assembler */


/*
 * Offsets of register values in saved context.
 */
#define PT_R1     0x00
#define PT_R2     0x04
#define PT_R3     0x08
#define PT_R4     0x0C
#define PT_R5     0x10
#define PT_R6     0x14
#define PT_R7     0x18
#define PT_R8     0x1C
#define PT_R9     0x20
#define PT_R10    0x24
#define PT_R11    0x28
#define PT_R12    0x2C
#define PT_R13    0x30
#define PT_R14    0x34
#define PT_R15    0x38
#define PT_R16    0x3C
#define PT_R17    0x40
#define PT_R18    0x44
#define PT_R19    0x48
#define PT_R20    0x4C
#define PT_R21    0x50
#define PT_R22    0x54
#define PT_R23    0x58
#define PT_R24    0x5C
#define PT_R25    0x60
#define PT_GP     0x64
#define PT_SP     0x68
#define PT_FP     0x6C
#define PT_RA     0x70

#define PT_LO     0x74
#define PT_HI     0x78
#define PT_STATUS 0x7C
#define PT_PC     0x80

/* sizeof(pt_regs) */
#define PT_SIZE   0x84

#endif /* __ASSEMBLER__ */


#endif /* MIPS_PTRACE_H_ */

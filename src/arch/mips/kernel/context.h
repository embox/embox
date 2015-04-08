/**
 * @file
 *
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef ARCH_MIPS_CONTEXT_H_
#define ARCH_MIPS_CONTEXT_H_

#ifndef __ASSEMBLER__

struct context {
	unsigned int s[9]; /* static registers s0-s8 */
	unsigned int gp;   /* global pointer */
	unsigned int ra;   /* return address */
	unsigned int sp;   /* stack pointer */
	unsigned int c0_stat; /* c0 coprocessor status */
};

#else

/* Offsets for struct context_kregs. */
#define CTX_S0     0x00
#define CTX_S1     0x04
#define CTX_S2     0x08
#define CTX_S3     0x0C
#define CTX_S4     0x10
#define CTX_S5     0x14
#define CTX_S6     0x18
#define CTX_S7     0x1C
#define CTX_S8     0x20
#define CTX_GP     0x24
#define CTX_RA     0x28
#define CTX_SP     0x2C
#define CTX_CR0_STAT 0x30

#define CTX_SIZE   0x34
#endif

#endif /* ARCH_MIPS_CONTEXT_H_ */

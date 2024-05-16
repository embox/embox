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
	unsigned long s[9];    /* static registers s0-s8 */
	unsigned long gp;      /* global pointer */
	unsigned long ra;      /* return address */
	unsigned long sp;      /* stack pointer */
	unsigned long c0_stat; /* c0 coprocessor status */
};

#endif /* !__ASSEMBLER__ */

#define CTX_S0       (LONGSIZE * 0)
#define CTX_S1       (LONGSIZE * 1)
#define CTX_S2       (LONGSIZE * 2)
#define CTX_S3       (LONGSIZE * 3)
#define CTX_S4       (LONGSIZE * 4)
#define CTX_S5       (LONGSIZE * 5)
#define CTX_S6       (LONGSIZE * 6)
#define CTX_S7       (LONGSIZE * 7)
#define CTX_S8       (LONGSIZE * 8)
#define CTX_GP       (LONGSIZE * 9)
#define CTX_RA       (LONGSIZE * 10)
#define CTX_SP       (LONGSIZE * 11)
#define CTX_CR0_STAT (LONGSIZE * 12)
#define CTX_SIZE     (LONGSIZE * 13)

#endif /* ARCH_MIPS_CONTEXT_H_ */

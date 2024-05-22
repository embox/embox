/**
 * @file
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_ASM_SETJMP_H_
#define MIPS_ASM_SETJMP_H_

#include <asm/asm.h>

#ifndef __ASSEMBLER__

typedef struct {
	unsigned long s[8]; /* static registers */
	unsigned long fp;   /* frame pointer */
	unsigned long ra;   /* return address */
	unsigned long gp;   /* global pointer */
	unsigned long sp;   /* stack pointer */
} __jmp_buf[1];

#endif /* !__ASSEMBLER__ */

#define SETJMP_S0 (LONGSIZE * 0)
#define SETJMP_S1 (LONGSIZE * 1)
#define SETJMP_S2 (LONGSIZE * 2)
#define SETJMP_S3 (LONGSIZE * 3)
#define SETJMP_S4 (LONGSIZE * 4)
#define SETJMP_S5 (LONGSIZE * 5)
#define SETJMP_S6 (LONGSIZE * 6)
#define SETJMP_S7 (LONGSIZE * 7)
#define SETJMP_FP (LONGSIZE * 8)
#define SETJMP_RA (LONGSIZE * 9)
#define SETJMP_GP (LONGSIZE * 10)
#define SETJMP_SP (LONGSIZE * 11)

#endif /* MIPS_ASM_SETJMP_H_ */

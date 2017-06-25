/**
 * @file
 *
 * @brief
 *
 * @date 22.06.2012
 * @author Anton Bondarev
 */

#ifndef MIPS_SETJMP_H_
#define MIPS_SETJMP_H_

#ifndef __ASSEMBLER__
typedef struct {
	unsigned int s[8];     /* static registers */
	unsigned int fp;       /* frame pointer */
	unsigned int ra;       /* return address */
	unsigned int gp;       /* global pointer */
	unsigned int sp;       /* stack pointer */
} __jmp_buf[1];

#else

#define SETJMP_S0 0x00
#define SETJMP_S1 0x04
#define SETJMP_S2 0x08
#define SETJMP_S3 0x0C
#define SETJMP_S4 0x10
#define SETJMP_S5 0x14
#define SETJMP_S6 0x18
#define SETJMP_S7 0x1C
#define SETJMP_FP 0x20
#define SETJMP_RA 0x24
#define SETJMP_GP 0x28
#define SETJMP_SP 0x2C


#endif

#endif /* MIPS_SETJMP_H_ */

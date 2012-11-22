/**
 * @file
 *
 * @date 22.06.12
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef PPC_SETJMP_H_
#define PPC_SETJMP_H_

#ifndef __ASSEMBLER__

typedef struct {
	unsigned int sp;    /* stack pointer */
	unsigned int tc;    /* table of contents pointer */
	unsigned int ra;    /* return address */
	unsigned int cr;    /* condition register */
	unsigned int s[19]; /* static registers */
} __jmp_buf[1];

#else /* __ASSEMBLER__ */

#define SETJMP_SP   0x00
#define SETJMP_TC   0x04
#define SETJMP_RA   0x08
#define SETJMP_CR   0x0C
#define SETJMP_S0   0x10
#define SETJMP_S1   0x14
#define SETJMP_S2   0x18
#define SETJMP_S3   0x1C
#define SETJMP_S4   0x20
#define SETJMP_S5   0x24
#define SETJMP_S6   0x28
#define SETJMP_S7   0x2C
#define SETJMP_S8   0x30
#define SETJMP_S9   0x34
#define SETJMP_S10  0x38
#define SETJMP_S11  0x3C
#define SETJMP_S12  0x40
#define SETJMP_S13  0x44
#define SETJMP_S14  0x48
#define SETJMP_S15  0x4C
#define SETJMP_S16  0x50
#define SETJMP_S17  0x54
#define SETJMP_S18  0x58

#define SETJMP_SIZE 0x5C

#endif /* !__ASSEMBLER__ */

#endif /* PPC_SETJMP_H_ */

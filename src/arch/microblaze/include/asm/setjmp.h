/**
 * @file
 * @brief Microblaze definition objects for setjmp
 *
 * @date 11.03.10
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#ifndef MICROBLAZE_SETJMP_H_
#define MICROBLAZE_SETJMP_H_

#ifndef __ASSEMBLER__

typedef struct {
	unsigned int sp;    /* stack pointer (r1) */
	unsigned int rsda;  /* read-only small data area anchor (r2) */
	unsigned int wsda;  /* read-write small data area anchor (r13) */
	unsigned int ra;    /* return address (r15) */
	unsigned int rzrv;  /* reserved for assembler (r18) */
	unsigned int s[13]; /* callee-save registers (r19..r31) */
} __jmp_buf[1];

#else /* __ASSEMBLER__ */

#define SETJMP_SP   0x00
#define SETJMP_RSDA 0x04
#define SETJMP_WSDA 0x08
#define SETJMP_RA   0x0C
#define SETJMP_RZRV 0x10
#define SETJMP_S19  0x14
#define SETJMP_S20  0x18
#define SETJMP_S21  0x1C
#define SETJMP_S22  0x20
#define SETJMP_S23  0x24
#define SETJMP_S24  0x28
#define SETJMP_S25  0x2C
#define SETJMP_S26  0x30
#define SETJMP_S27  0x34
#define SETJMP_S28  0x38
#define SETJMP_S29  0x3C
#define SETJMP_S30  0x40
#define SETJMP_S31  0x44

#define SETJMP_SIZE 0x48

#endif /* !__ASSEMBLER__ */

#endif /* MICROBLAZE_SETJMP_H_ */

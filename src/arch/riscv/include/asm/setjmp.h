/**
 * @file
 * @brief setjmp/longjmp for the riscv architecture
 *
 * @date 30.09.19
 * @author Anastasia Nizharadze
 */

#ifndef RISCV_SETJMP_H_
#define RISCV_SETJMP_H_

#ifndef __ASSEMBLER__

typedef struct {
	unsigned long s[12]; /* static registers */
	unsigned long ra;    /* return address */
	unsigned long sp;    /* stack pointer */
} __jmp_buf[1];

#else /* !__ASSEMBLER__ */

#define SETJMP_S0  (SZREG * 0)
#define SETJMP_S1  (SZREG * 1)
#define SETJMP_S2  (SZREG * 2)
#define SETJMP_S3  (SZREG * 3)
#define SETJMP_S4  (SZREG * 4)
#define SETJMP_S5  (SZREG * 5)
#define SETJMP_S6  (SZREG * 6)
#define SETJMP_S7  (SZREG * 7)
#define SETJMP_S8  (SZREG * 8)
#define SETJMP_S9  (SZREG * 9)
#define SETJMP_S10 (SZREG * 10)
#define SETJMP_S11 (SZREG * 11)
#define SETJMP_RA  (SZREG * 12)
#define SETJMP_SP  (SZREG * 13)

#endif /* __ASSEMBLER__ */

#endif /* RISCV_SETJMP_H_ */

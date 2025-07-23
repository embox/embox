/**
 * @file
 * @brief setjmp/longjmp for the riscv architecture
 *
 * @date 30.09.19
 * @author Anastasia Nizharadze
 */

#ifndef RISCV_ASM_SETJMP_H_
#define RISCV_ASM_SETJMP_H_

typedef struct {
	unsigned long ra;    /* return address */
	unsigned long sp;    /* stack pointer */
	unsigned long s[12]; /* static registers */
} __jmp_buf[1];

#endif /* RISCV_ASM_SETJMP_H_ */

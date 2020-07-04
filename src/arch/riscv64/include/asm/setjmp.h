/**
 * @file
 * @brief setjmp/longjmp for the riscv architecture
 *
 * @date 30.09.19
 * @author Anastasia Nizharadze
 */

#ifndef RISCV64_SETJMP_H_
#define RISCV64_SETJMP_H_

#ifndef __ASSEMBLER__

typedef struct {
	unsigned int s[12];     /* static registers */
	unsigned int ra;       /* return address */
	unsigned int sp;       /* stack pointer */
} __jmp_buf[1];

#else /* __ASSEMBLER__ */
#if __riscv_xlen == 32

#define SETJMP_S0    0x00
#define SETJMP_S1    0x04
#define SETJMP_S2    0x08
#define SETJMP_S3    0x0C
#define SETJMP_S4    0x10
#define SETJMP_S5    0x14
#define SETJMP_S6    0x18
#define SETJMP_S7    0x1C
#define SETJMP_S8    0x20
#define SETJMP_S9    0x24
#define SETJMP_S10   0x28
#define SETJMP_S11   0x2C
#define SETJMP_RA    0x30
#define SETJMP_SP    0x34

#elif __riscv_xlen == 64

#define SETJMP_S0    0x00
#define SETJMP_S1    0x08
#define SETJMP_S2    0x10
#define SETJMP_S3    0x18
#define SETJMP_S4    0x20
#define SETJMP_S5    0x28
#define SETJMP_S6    0x30
#define SETJMP_S7    0x38
#define SETJMP_S8    0x40
#define SETJMP_S9    0x48
#define SETJMP_S10   0x50
#define SETJMP_S11   0x58
#define SETJMP_RA    0x60
#define SETJMP_SP    0x68

#else
#error Unsupported arch
#endif

#endif /* __ASSEMBLER__ */

#endif /* RISCV64_SETJMP_H_ */

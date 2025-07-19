/**
 * @file
 *
 * @brief
 *
 * @date 21.01.2020
 * @author Nastya Nizharadze
 */

#ifndef ARCH_RISCV_CONTEXT_H_
#define ARCH_RISCV_CONTEXT_H_

#include <asm/asm.h>

#ifndef __ASSEMBLER__

struct context {
	unsigned long ra;
	unsigned long sp;
	unsigned long s[12];
	unsigned long status;
};

#else /* !__ASSEMBLER__ */

#define CTX_RA     (REG_SIZE_X * 0)
#define CTX_SP     (REG_SIZE_X * 1)
#define CTX_S0     (REG_SIZE_X * 2)
#define CTX_S1     (REG_SIZE_X * 3)
#define CTX_S2     (REG_SIZE_X * 4)
#define CTX_S3     (REG_SIZE_X * 5)
#define CTX_S4     (REG_SIZE_X * 6)
#define CTX_S5     (REG_SIZE_X * 7)
#define CTX_S6     (REG_SIZE_X * 8)
#define CTX_S7     (REG_SIZE_X * 9)
#define CTX_S8     (REG_SIZE_X * 10)
#define CTX_S9     (REG_SIZE_X * 11)
#define CTX_S10    (REG_SIZE_X * 12)
#define CTX_S11    (REG_SIZE_X * 13)
#define CTX_STATUS (REG_SIZE_X * 14)

#endif /* __ASSEMBLER__ */

#endif /* ARCH_RISCV_CONTEXT_H_ */

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

#define CTX_RA     (SZREG * 0)
#define CTX_SP     (SZREG * 1)
#define CTX_S0     (SZREG * 2)
#define CTX_S1     (SZREG * 3)
#define CTX_S2     (SZREG * 4)
#define CTX_S3     (SZREG * 5)
#define CTX_S4     (SZREG * 6)
#define CTX_S5     (SZREG * 7)
#define CTX_S6     (SZREG * 8)
#define CTX_S7     (SZREG * 9)
#define CTX_S8     (SZREG * 10)
#define CTX_S9     (SZREG * 11)
#define CTX_S10    (SZREG * 12)
#define CTX_S11    (SZREG * 13)
#define CTX_STATUS (SZREG * 14)

#endif /* __ASSEMBLER__ */

#endif /* ARCH_RISCV_CONTEXT_H_ */

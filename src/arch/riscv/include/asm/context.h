/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 18.07.25
 */

#ifndef RISCV_ASM_CONTEXT_H_
#define RISCV_ASM_CONTEXT_H_

#include <asm/asm.h>

#define CALLER_SAVED_SIZE_X (REG_SIZE_X * 15)
#define CALLEE_SAVED_SIZE_X (REG_SIZE_X * 14)
#define CALLER_SAVED_SIZE_F (REG_SIZE_F * 20)
#define CALLEE_SAVED_SIZE_F (REG_SIZE_F * 12)

#ifdef __ASSEMBLER__
#include "context_macros.s"
#else
struct caller_saved_x {
	unsigned long a[8];
	unsigned long t[7];
};

struct callee_saved_x {
	unsigned long ra;
	unsigned long sp;
	unsigned long s[12];
};

struct caller_saved_f {
#if REG_SIZE_F == 4
	float fa[8];
	float ft[12];
#elif REG_SIZE_F == 8
	double fa[8];
	double ft[12];
#endif
};

struct callee_saved_f {
#if REG_SIZE_F == 4
	float fs[12];
#elif REG_SIZE_F == 8
	double fs[12];
#endif
};
#endif /* __ASSEMBLER__ */

#endif /* RISCV_ASM_CONTEXT_H_ */

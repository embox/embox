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

#define CALLER_SAVED_REGS_SIZE (REG_SIZE_X * 15 + REG_SIZE_F * 20)
#define CALLEE_SAVED_REGS_SIZE (REG_SIZE_X * 14 + REG_SIZE_F * 12)

#ifdef __ASSEMBLER__
#include "context_macros.s"
#else
#include <assert.h>
#include <stddef.h>

struct caller_saved_regs {
#if REG_SIZE_F == 4
	float fa[8];
	float ft[12];
#elif REG_SIZE_F == 8
	double fa[8];
	double ft[12];
#endif
	unsigned long a[8];
	unsigned long t[7];
};

struct callee_saved_regs {
#if REG_SIZE_F == 4
	float fs[12];
#elif REG_SIZE_F == 8
	double fs[12];
#endif
	unsigned long ra;
	unsigned long sp;
	unsigned long s[12];
};

/* Check that the structures are the correct size and do not contain padding bytes.*/
static_assert(sizeof(struct caller_saved_regs) == CALLER_SAVED_REGS_SIZE,
    "error: Invalid sizeof(struct caller_saved_regs)");
static_assert(sizeof(struct callee_saved_regs) == CALLEE_SAVED_REGS_SIZE,
    "error: Invalid sizeof(struct callee_saved_regs)");

#endif /* __ASSEMBLER__ */

#endif /* RISCV_ASM_CONTEXT_H_ */

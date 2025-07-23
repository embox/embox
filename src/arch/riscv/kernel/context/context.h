/**
 * @file
 * @brief
 *
 * @date 21.01.2020
 * @author Nastya Nizharadze
 */

#ifndef ARCH_RISCV_CONTEXT_H_
#define ARCH_RISCV_CONTEXT_H_

#include <asm/context.h>

/* All callee-saved and [m|s]status registers */
struct context {
	struct callee_saved_x x;
	struct callee_saved_f f;
	unsigned long status;
};

#endif /* ARCH_RISCV_CONTEXT_H_ */

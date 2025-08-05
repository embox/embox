/**
 * @file
 * @brief
 *
 * @date 21.01.2020
 * @author Nastya Nizharadze
 */

#ifndef ARCH_RISCV_CONTEXT_H_
#define ARCH_RISCV_CONTEXT_H_

#include <stdint.h>

#include <asm/context.h>

/* All callee-saved and [m|s]status registers */
struct context {
	struct callee_saved_regs r;
	unsigned long status;
#ifdef __riscv_f
	uint32_t fcsr;
#endif
};

#endif /* ARCH_RISCV_CONTEXT_H_ */

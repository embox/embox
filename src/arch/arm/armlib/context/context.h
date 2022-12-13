/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.10.2012
 */

#ifndef SRC_ARCH_ARM_CORTEXA8_KERNEL_CONTEXT_H_
#define SRC_ARCH_ARM_CORTEXA8_KERNEL_CONTEXT_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

#include <asm/ptrace.h>
#include <arm/fpu.h>

struct context {
	pt_regs_t ptregs;
	fpu_context_t fpu_context;
};

#endif /* __ASSEMBLER__ */

#endif /* SRC_ARCH_ARM_CORTEXA8_KERNEL_CONTEXT_H_ */

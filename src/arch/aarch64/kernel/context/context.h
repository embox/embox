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

struct context {
	uintptr_t system_r[13];
	uintptr_t sp;
	uintptr_t lr;
	uintptr_t cpsr;
};

#endif /* __ASSEMBLER__ */

#endif /* SRC_ARCH_ARM_CORTEXA8_KERNEL_CONTEXT_H_ */

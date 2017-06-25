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

#define CM3_CONTROL_NPRIV     0x00000000
#define CM3_CONTROL_SPSEL_PSP 0x00000001

struct context {
	uint32_t r[13];
	uint32_t lr;
	uint32_t sp;
	uint32_t control;
};

#endif /* __ASSEMBLER__ */

#endif /* SRC_ARCH_ARM_CORTEXA8_KERNEL_CONTEXT_H_ */


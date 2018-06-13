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
#include <arm/fpu.h>

#define CM3_CONTROL_NPRIV     0x00000000
#define CM3_CONTROL_SPSEL_PSP 0x00000001

struct context {
	uint32_t r[13];
	uint32_t lr;
	uint32_t sp;
	uint32_t control;
	uint32_t fpu_data[FPU_DATA_LEN];
};

/* Context saved by CPU on exception entering */
struct exc_saved_base_ctx {
	uint32_t r[5];
	uint32_t lr;
	uint32_t pc;
	uint32_t psr;
};

#endif /* __ASSEMBLER__ */

#endif /* SRC_ARCH_ARM_CORTEXA8_KERNEL_CONTEXT_H_ */


/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.10.2012
 */
#ifndef ARCH_AARCH64_CONTEXT_H_
#define ARCH_AARCH64_CONTEXT_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

struct context {
	uint64_t x[10] /* X19-X28 */;
	uint64_t fp;
	uint64_t lr;
	uint64_t sp;
	uint64_t spsr;
	uint64_t daif;
};

#endif /* __ASSEMBLER__ */

#endif /* ARCH_AARCH64_CONTEXT_H_ */

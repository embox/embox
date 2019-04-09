/**
 * @file
 *
 * @date 18.03.2019
 * @author Alexander Kalmuk
 */

#ifndef ARCH_E2K_CONTEXT_H_
#define ARCH_E2K_CONTEXT_H_

/* Specify that context_init() requires the stack size to be passed
 * as an argument. Used in src/include/hal/context.h */
#define CONTEXT_USE_STACK_SIZE 1

#ifndef __ASSEMBLER__

#include <stdint.h>

struct context {
	/* CRs */
	uint64_t cr0_lo;
	uint64_t cr0_hi;
	uint64_t cr1_lo;
	uint64_t cr1_hi;

	/* Stacks */
	uint64_t usd_lo;
	uint64_t usd_hi;
	uint64_t psp_lo;
	uint64_t psp_hi;
	uint64_t pcsp_lo;
	uint64_t pcsp_hi;

	/* IPL control */
	uint64_t upsr;
};

#endif /* __ASSEMBLER__ */

/* Offset within struct context */
#define E2K_CTX_CR0_LO  0x00
#define E2K_CTX_CR0_HI  0x08
#define E2K_CTX_CR1_LO  0x10
#define E2K_CTX_CR1_HI  0x18
#define E2K_CTX_USD_LO  0x20
#define E2K_CTX_USD_HI  0x28
#define E2K_CTX_PSP_LO  0x30
#define E2K_CTX_PSP_HI  0x38
#define E2K_CTX_PCSP_LO 0x40
#define E2K_CTX_PCSP_HI 0x48
#define E2K_CTX_UPSR    0x50

#endif /* ARCH_E2K_CONTEXT_H_ */

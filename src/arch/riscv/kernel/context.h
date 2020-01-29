/**
 * @file
 *
 * @brief
 *
 * @date 21.01.2020
 * @author Nastya Nizharadze
 */

#ifndef ARCH_RISCV_CONTEXT_H_
#define ARCH_RISCV_CONTEXT_H_

#include <stdint.h>

#ifndef __ASSEMBLER__

struct context {
	uint32_t ra;
	uint32_t sp;
	uint32_t s[12];
	uint32_t mstatus;
};

#endif

#define CTX_RA      0x00
#define CTX_SP      0x04
#define CTX_S0      0x08
#define CTX_S1      0x0C
#define CTX_S2      0x10
#define CTX_S3      0x14
#define CTX_S4      0x18
#define CTX_S5      0x1C
#define CTX_S6      0x20
#define CTX_S7      0x24
#define CTX_S8      0x28
#define CTX_S9      0x2C
#define CTX_S10     0x30
#define CTX_S11     0x34
#define CTX_MSTATUS 0x38

#define CTX_SIZE    0x3c

#endif /* ARCH_RISCV_CONTEXT_H_ */

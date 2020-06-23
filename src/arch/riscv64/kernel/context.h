/**
 * @file
 *
 * @brief
 *
 * @date 23.06.2020
 * @author sksat
 */

#ifndef ARCH_RISCV64_CONTEXT_H_
#define ARCH_RISCV64_CONTEXT_H_

#include <stdint.h>

#ifndef __ASSEMBLER__

struct context {
	uint64_t ra;
	uint64_t sp;
	uint64_t s[12];
	uint64_t mstatus;
};

#endif

#define CTX_RA      0x00
#define CTX_SP      0x08
#define CTX_S0      0x10
#define CTX_S1      0x18
#define CTX_S2      0x20
#define CTX_S3      0x28
#define CTX_S4      0x30
#define CTX_S5      0x38
#define CTX_S6      0x40
#define CTX_S7      0x48
#define CTX_S8      0x50
#define CTX_S9      0x58
#define CTX_S10     0x60
#define CTX_S11     0x68
#define CTX_MSTATUS 0x70

#define CTX_SIZE    0x78

#endif /* ARCH_RISCV64_CONTEXT_H_ */

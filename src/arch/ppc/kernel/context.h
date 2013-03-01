/**
 * @file
 *
 * @date 14.11.12
 * @author Ilia Vaprol
 */

#ifndef ARCH_PPC_CONTEXT_H_
#define ARCH_PPC_CONTEXT_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

struct context {
	uint32_t gpr[19]; /* General Purpose Register 13..31 */
	uint32_t lr;      /* Link Register */
	uint32_t cr;      /* Condition Register */
	uint32_t sp;      /* Stack Pointer */
	uint32_t tc;      /* Table of Contents */
};

#else /* __ASSEMBLER__ */

#define CTX_GPR13 0x00
#define CTX_GPR14 0x04
#define CTX_GPR15 0x08
#define CTX_GPR16 0x0C
#define CTX_GPR17 0x10
#define CTX_GPR18 0x14
#define CTX_GPR19 0x18
#define CTX_GPR20 0x1C
#define CTX_GPR21 0x20
#define CTX_GPR22 0x24
#define CTX_GPR23 0x28
#define CTX_GPR24 0x2C
#define CTX_GPR25 0x30
#define CTX_GPR26 0x34
#define CTX_GPR27 0x38
#define CTX_GPR28 0x3C
#define CTX_GPR29 0x40
#define CTX_GPR30 0x44
#define CTX_GPR31 0x48
#define CTX_LR    0x4C
#define CTX_CR    0x50
#define CTX_SP    0x54
#define CTX_TC    0x58

#define CTX_SIZE  0x5C

#endif /* !__ASSEMBLER__ */

#endif /* ARCH_PPC_CONTEXT_H_ */

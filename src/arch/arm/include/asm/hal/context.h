#ifndef HAL_CONTEXT_H_
# error "Do not include this file directly!"
#endif /* HAL_CONTEXT_H_ */

#include <types.h>

#ifndef __ASSEMBLER__

struct context_kregs {
	/* 0x00 */uint32_t lr; /**< Return address for #context_switch(). */
	/* 0x04 */uint32_t cpsr; /**< State register */
	/* 0x08 */uint32_t sp; /**< Stack pointer */
};

struct context {
	struct context_kregs kregs;
	struct pt_regs *uregs;
};

#else /* __ASSEMBLER__ */

/* Offsets for struct context_kregs. */
#define CTX_LR      0x00
#define CTX_CPSR    0x04
#define CTX_SP      0x08

#endif /* __ASSEMBLER__ */


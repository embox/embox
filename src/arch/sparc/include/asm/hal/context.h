#ifndef HAL_CONTEXT_H_
# error "Do not include this file directly!"
#endif /* HAL_CONTEXT_H_ */

#include <types.h>

#ifndef __ASSEMBLER__

struct context_kregs {
	/* 0x00 */uint32_t ksp; /**< Kernel stack pointer. */
	/* 0x04 */uint32_t ret; /**< Return address for #context_switch(). */
	/* 0x08 */uint32_t arg; /**< TODO For initial context switch. -- Eldar */
	/* 0x0c */uint32_t psr; /**< At least for CWP value. */
	/* 0x10 */uint32_t wim; /**< Context switch time invalid window. */
};

struct context {
	struct context_kregs kregs;
	struct pt_regs *uregs;
};

#else /* __ASSEMBLER__ */

/* Offsets for struct context_kregs. */
#define CTX_KSP    0x00
#define CTX_RET    0x04
#define CTX_ARG    0x08
#define CTX_PSR    0x0c
#define CTX_WIM    0x10

#endif /* __ASSEMBLER__ */

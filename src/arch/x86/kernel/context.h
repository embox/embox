/**
 * @file
 *
 * @date 01.12.10
 * @author Nikolay Korotkiy
 */

#ifndef ARCH_X86_CONTEXT_H_
#define ARCH_X86_CONTEXT_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

struct context {
	uint32_t eip;    /* instruction pointer */
	uint32_t ebx;    /* base register */
	uint32_t edi;    /* destination index register */
	uint32_t esi;    /* source index register */
	uint32_t ebp;    /* stack pointer register */
	uint32_t esp;    /* stack base pointer register */
	uint32_t eflags; /* hold the state of the processor */
#ifdef __SSE__
	/* fxsave_region has extra 8 bytes for 16-byte alignment */
	uint32_t cr4;               /* SSE extension control register */
	uint64_t fxsave_region[65]; /* SSE, x87 FPU, and MMX state */
#endif
};

#endif /* __ASSEMBLER__ */

#define CTX_X86_EIP    0x00 /* offsetof(struct context,eip) */
#define CTX_X86_EBX    0x04 /* offsetof(struct context,ebx) */
#define CTX_X86_EDI    0x08 /* offsetof(struct context,edi) */
#define CTX_X86_ESI    0x0c /* offsetof(struct context,esi) */
#define CTX_X86_EBP    0x10 /* offsetof(struct context,ebp) */
#define CTX_X86_ESP    0x14 /* offsetof(struct context,esp) */
#define CTX_X86_EFLAGS 0x18 /* offsetof(struct context,eflags) */

#endif /* ARCH_X86_CONTEXT_H_ */

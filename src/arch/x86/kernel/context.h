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
/*
 * General registers
 * EAX EBX ECX EDX
 *
 * Segment registers
 * CS DS ES FS GS SS
 *
 * Index and pointers
 * ESI EDI EBP EIP ESP
 *
 * Indicator
 * EFLAGS
 */
struct context {
	/* 0x00 */uint32_t eip; /**< instruction pointer */
	/* 0x04 */uint32_t ebx; /**< base register */
	/* 0x08 */uint32_t edi; /**< Destination index register */
	/* 0x0c */uint32_t esi; /**< Source index register */
	/* 0x10 */uint32_t ebp; /**< Stack pointer register */
	/* 0x14 */uint32_t esp; /**< Stack Base pointer register */
	/* 0x18 */uint32_t eflags; /**< EFLAGS register hold the state of the processor */
};


#endif /* __ASSEMBLER__ */

#define CTX_X86_EBX    0x04 /* offsetof(struct context,ebx) */
#define CTX_X86_EIP    0x00 /* offsetof(struct context,eip) */
#define CTX_X86_EDI    0x08 /* offsetof(struct context,edi) */
#define CTX_X86_ESI    0x0C /* offsetof(struct context,esi) */
#define CTX_X86_EBP    0x10 /* offsetof(struct context,ebp) */
#define CTX_X86_ESP    0x14 /* offsetof(struct context,esp) */
#define CTX_X86_EFLAGS 0x18 /* offsetof(struct context,eflags) */

#endif /* ARCH_X86_CONTEXT_H_ */

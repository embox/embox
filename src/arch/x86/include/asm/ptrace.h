/**
 * @file
 *
 * @date May 26, 2014
 * @author: Anton Bondarev
 */

#ifndef X86_PTRACE_H_
#define X86_PTRACE_H_

#include <stdint.h>

#ifndef __ASSEMBLER__

typedef struct pt_regs {
	/* Ranged in reverse order of SAVE_ALL */
	uint32_t ebx;
	uint32_t ecx;
	uint32_t edx;
	uint32_t esi;
	uint32_t edi;
	uint32_t ebp;
	uint32_t eax;
	uint32_t gs;
	uint32_t fs;
	uint32_t es;
	uint32_t ds;

	/* Pushed at the very beginning of entry. */
	uint32_t trapno;

	/* In some cases pushed by processor, in some - by us. */
	uint32_t err;

	/* Pushed by processor. */
	uint32_t eip;
	uint32_t cs;
	uint32_t eflags;

	/* Pushed by processor, if switching of rings occurs. */
	uint32_t esp;
	uint32_t ss;
} pt_regs_t;

static inline void ptregs_retcode(struct pt_regs *ptregs, int retcode) {
	ptregs->eax = retcode;
}

#else

/* Use for exception which doesn't push error code. */
#define EXCEPTION(n, name)  \
	.globl name            ;\
name:                      ;\
	pushl	$(0)           ;\
	pushl	$(n)           ;\
	jmp	excep_stub

/* Use for exception which pushes error code. */
#define EXCEPTION_ERR(n, name)  \
	.globl name                ;\
name:                          ;\
	pushl	$(n)               ;\
	jmp	excep_stub

#define IRQ_ENTRY(n)      \
	.globl irq##n        ;\
irq##n:                  ;\
	pushl   $(0)         ;\
	pushl   $(32 + n)    ;\
	jmp     irq_stub

#endif /* __ASSEMBLER__ */


#endif /* X86_PTRACE_H_ */

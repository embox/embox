/**
 * @file
 * @brief Definitions used by low-level trap handlers
 *
 * @date 21.09.11
 * @author Anton Bondarev
 */

#ifndef X86_ENTRY_H_
#define X86_ENTRY_H_

#include <asm/gdt.h>

#ifdef __ASSEMBLER__

#define SETUP_SEGMENTS            \
	movl    $(__KERNEL_DS), %edx; \
	movl    %edx, %ds;            \
	movl    %edx, %es;

#define SAVE_ALL     \
	pushl   %ds;     \
	pushl   %es;     \
	pushl   %fs;     \
	pushl   %gs;     \
	pushl   %eax;    \
	pushl   %ebp;    \
	pushl   %edi;    \
	pushl   %esi;    \
	pushl   %edx;    \
	pushl   %ecx;    \
	pushl   %ebx;    \
	SETUP_SEGMENTS;

#define RESTORE_ALL  \
	pop   %ebx;      \
	pop   %ecx;      \
	pop   %edx;      \
	pop   %esi;      \
	pop   %edi;      \
	pop   %ebp;      \
	pop   %eax;      \
	pop   %gs;       \
	pop   %fs;       \
	pop   %es;       \
	pop   %ds;       \
	add   $8, %esp;  \
	iret;

#endif /* __ASSEMBLER__ */

#endif /* X86_ENTRY_H_ */

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
	movw    $(__KERNEL_DS), %dx;  \
	movw    %dx, %ds;             \
	movw    %dx, %es;

#define SAVE_ALL_REGS \
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
	pushl   %ebx;

#define SAVE_ALL     \
	SAVE_ALL_REGS \
	SETUP_SEGMENTS;

#define RESTORE_ALL_REGS \
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
	pop   %ds;

#define RESTORE_ALL  \
	RESTORE_ALL_REGS \
	add   $8, %esp;  \
	iret;

/*
 *  The order in which registers are stored in the pt_regs structure
 */

#define PT_EBX     0
#define PT_ECX     4
#define PT_EDX     8
#define PT_ESI     12
#define PT_EDI     16
#define PT_EBP     20
#define PT_EAX     24
#define PT_GS      28
#define PT_FS      32
#define PT_ES      36
#define PT_DS      40

#define PT_TRAPNO  44
#define PT_ERR     48

#define PT_EIP     52
#define PT_CS      56
#define PT_EFLAGS  60
#define PT_ESP     64
#define PT_SS      68

#define PT_END     72

#endif /* __ASSEMBLER__ */

#endif /* X86_ENTRY_H_ */

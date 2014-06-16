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

/*
 *  The order in which registers are stored in the pt_regs structure
 */

#define PT_EBX     0
#define PT_ECX     1
#define PT_EDX     2
#define PT_ESI     3
#define PT_EDI     4
#define PT_EBP     5
#define PT_EAX     6
#define PT_GS      7
#define PT_FS      8
#define PT_ES      9
#define PT_DS      10

#define PT_TRAPNO  11
#define PT_ERR     12

#define PT_EIP     13
#define PT_CS      14
#define PT_EFLAGS  15
#define PT_ESP     16
#define PT_SS      17

#endif /* __ASSEMBLER__ */

#endif /* X86_ENTRY_H_ */

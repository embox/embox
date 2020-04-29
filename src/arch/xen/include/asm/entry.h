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

#define CALL_WPTREGS \
	push    %ebp;               \
	movl    %esp, %ebp;         \
	subl    $28, %esp;          \
	SAVE_ALL_REGS;              \
	movl    %esp, %eax;         \
	addl    $PT_EBP, %eax;      \
	push    SAVED_EBP(%esp);    \
	popl    (%eax);             \
	movl    FRAME_END(%esp), %ecx; \
	movl    %ecx, PT_EIP(%esp); \
	push    %cs;                \
	popl    PT_CS(%esp);        \
	pushf;                      \
	popl    PT_EFLAGS(%esp);    \
	movl    %esp, %eax;         \
	addl    $FRAME_END+4, %eax; \
	movl    %eax, PT_ESP(%esp); \
	push    %esp;               \
	call

/*
 *  The order in which registers are stored in the pt_regs structure
 */

#define PT_EBX     0
#define PT_EBX_N   0
#define PT_ECX     4
#define PT_ECX_N   1
#define PT_EDX     8
#define PT_EDX_N   2
#define PT_ESI     12
#define PT_ESI_N   3
#define PT_EDI     16
#define PT_EDI_N   4
#define PT_EBP     20
#define PT_EBP_N   5
#define PT_EAX     24
#define PT_EAX_N   6
#define PT_GS      28
#define PT_GS_N    7
#define PT_FS      32
#define PT_FS_N    8
#define PT_ES      36
#define PT_ES_N    9
#define PT_DS      40
#define PT_DS_N    10

#define PT_TRAPNO  44
#define PT_TRAPNO_N 11
#define PT_ERR     48
#define PT_ERR_N   12

#define PT_EIP     52
#define PT_EIP_N   13
#define PT_CS      56
#define PT_CS_N    14
#define PT_EFLAGS  60
#define PT_EFLAGS_N 15
#define PT_ESP     64
#define PT_ESP_N   16
#define PT_SS      68
#define PT_SS_N    17

#define SAVED_EBP     72
#define SAVED_EBP_N   18

#define FRAME_END    76
#define FRAME_END_N   19

#endif /* __ASSEMBLER__ */

#endif /* X86_ENTRY_H_ */

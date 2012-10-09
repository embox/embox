/**
 * @file
 * @brief Definitions used by low-level trap handlers
 *
 * @date 21.09.11
 * @author Anton Bondarev
 */

#ifndef X86_ENTRY_H_
#define X86_ENTRY_H_

#ifdef __ASSEMBLER__

#define SETUP_SEGMENTS \
	movl    %ss, %eax; \
	movl    %eax, %ds; \
	movl    %eax, %es;

#define SAVE_ALL     \
	pushl   %eax;    \
	pushl   %ecx;    \
	pushl   %edx;    \
	pushl   %ebx;    \
	pushl   %ebp;    \
	pushl   %esi;    \
	pushl   %edi;    \
	pushl   %ds;     \
	pushl   %es;     \
	pushl   %fs;     \
	pushl   %gs;     \
	SETUP_SEGMENTS;

#define RESTORE_ALL  \
	pop   %gs;       \
	pop   %fs;       \
	pop   %es;       \
	pop   %ds;       \
	pop   %edi;      \
	pop   %esi;      \
	pop   %ebp;      \
	pop   %ebx;      \
	pop   %edx;      \
	pop   %ecx;      \
	pop   %eax;      \
	add   $8, %esp;  \
	iret;

#endif /* __ASSEMBLER__ */

#endif /* X86_ENTRY_H_ */

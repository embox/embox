/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.05.2014
 */

#include <asm/traps.h>
#include <asm/entry.h>

	.text
	.extern vfork_body
	.global vfork
	.global vfork_leave

vfork:
	subl 	$28, %esp
	SAVE_ALL_REGS

	movl	PT_END(%esp), %ecx
	movl	%ecx, PT_EIP(%esp)

	pushf
	popl	PT_EFLAGS(%esp)

	movl	%esp, %eax
	addl	$PT_END+4, %eax
	movl	%eax, PT_ESP(%esp)

	push    %esp
	call    vfork_body

vfork_leave:
	movl	4(%esp), %ebp
	movl	PT_EIP(%ebp), %ecx
	movl	PT_ESP(%ebp), %edx
	subl	$4, %edx
	movl	%ecx, (%edx)

	movl	%ebp, %esp
	RESTORE_ALL_REGS

	addl 	$16, %esp
	popf
	movl 	(%esp), %esp
	subl	$4, %esp
	ret

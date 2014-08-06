/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.05.2014
 */

#include <asm/traps.h>
#include <asm/entry.h>

	.global ptregs_jmp

ptregs_jmp:
	movl	4(%esp), %esi
	movl	PT_ESP(%esi), %ebx

	movl	PT_EIP(%esi), %eax
	subl	$4, %ebx
	movl	%eax, (%ebx)

	subl	$PT_TRAPNO, %ebx
	movl	$PT_TRAPNO, %ecx
	movl	%ebx, %edi
	cld
	rep 	movsb

	movl	%ebx, %esp
	RESTORE_ALL_REGS
	ret

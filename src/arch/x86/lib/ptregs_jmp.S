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
	movl	%eax, %esi
	addl	$PT_ESP, %esi
	movl	PT_ESP(%eax), %edi

	movl	PT_ESP(%eax), %esp
	movl	$0x10, %ecx
	subl	$PT_ESP, %esp
	std
	rep		movsd

	RESTORE_ALL


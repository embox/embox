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
	movl    4(%esp), %eax

	/* Need to set new ESP now, correct value
	 * in pt_regs can be overwriten by movsd */
	movl    PT_ESP(%eax),%esp
	subl    $PT_SS, %esp

	movl    %eax, %esi
	addl    $PT_ESP, %esi
	movl    PT_ESP(%eax), %edi
	subl    $4, %edi
	movl    $PT_SS_N, %ecx
	std
	rep     movsd
	cld

	RESTORE_ALL

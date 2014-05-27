/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.05.2014
 */

#include <asm/traps.h>
#include <asm/entry.h>

	.text
	.extern fork_body
	.global fork
	.global stack_safety_point

fork:
	CALL_WPTREGS fork_body

stack_safety_point:
	movl %esp, %eax
	addl $4, %eax
	ret




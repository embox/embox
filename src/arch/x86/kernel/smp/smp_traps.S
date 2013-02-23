/**
 * @file
 * @brief
 *
 * @date 06.10.2012
 * @author Anton Bulychev
 */

#include <asm/traps.h>
#include <asm/entry.h>

	.text

	.globl resched_trap
resched_trap:
	pushl   $(0)
	pushl   $(0x80)
	jmp	resched_stub


	.section .traps.text

resched_stub:
	SAVE_ALL

	call resched

	RESTORE_ALL

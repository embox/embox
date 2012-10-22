/**
 * @file
 * @brief
 *
 * @date 10.10.2012
 * @author Anton Bulychev
 */

#include <asm/traps.h>
#include <asm/entry.h>

	.text

	.globl syscall_trap
syscall_trap:
	pushl   $(0)
	pushl   $(0x80)
	jmp	syscall_stub


	.section .traps.text

syscall_stub:
	SAVE_ALL

	/* TODO: Check %eax for good number of syscall. */

	call *SYSCALL_TABLE(,%eax,4)

	mov %eax, (PT_EAX*4)(%esp)  /* Save the return value */
	RESTORE_ALL

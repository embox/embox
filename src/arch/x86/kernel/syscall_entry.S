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


syscall_stub:
	SAVE_ALL

	mov	    %esp, %eax

	.global syscall_handler_return_pointer
	call    syscall_handler
syscall_handler_return_pointer:

	RESTORE_ALL

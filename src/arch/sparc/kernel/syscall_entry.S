/**
 * @file
 *
 * @date 07.11.2012
 * @author Anton Bulychev
 */

#include <asm/entry.h>
#include <asm/psr.h>
#include <asm/stack.h>

	.text
	.align 4

	.global syscall_entry
syscall_entry:

	SAVE_ALL

	or %t_psr, PSR_PIL, %temp
	wr %temp, %g0, %psr
	 WRITE_PAUSE
	wr %temp, PSR_ET, %psr
	 WRITE_PAUSE

	call syscall_handler
	 add %sp, STACK_FRAME_SZ, %o0

	/* Disable traps. */
	wr %temp, %g0, %psr
	 WRITE_PAUSE

	RESTORE_ALL

/**
 * @file
 * @brief
 *
 * @date 09.03.17
 * @author Anton Bondarev
 */

#include <asm/entry.h>
#include <asm/ptrace.h>

	.text
	.align 4

	.global vfork
vfork:
	SAVE_ALL

	call vfork_body
	 add %sp, STACK_FRAME_SZ, %o0

	nop /* noreturn*/


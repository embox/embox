/**
 * @file
 *
 * @date 09.03.17
 * @author Anton Bondarev
 */

#include <asm/linkage.h>
#include <asm/setjmp.h>
#include <asm/ptrace.h>

	.section .text

C_ENTRY(vfork):
	addik   r1, r1, -STATE_SAVE_SIZE; /* Make room on the stack. */
	SAVE_REGS

	swi	r1, r1, PT_R1;

	brlid   r15, vfork_body
	addik   r5, r1, PT_OFFSET  /* set first param */


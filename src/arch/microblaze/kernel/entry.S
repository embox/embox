/**
 * @file
 * @brief Low-level system-call handling, trap handlers and context-switching
 *
 * @date 25.11.09
 * @author Anton Bondarev
 */

#include <asm/linkage.h>
#include <asm/ptrace.h>
#include <asm/regs.h>

	.section .text

C_ENTRY(reset_hnd):
	brai _start

C_ENTRY(user_except_hnd):
C_ENTRY(break_except_hnd):
	addik   r1, r1, -STATE_SAVE_SIZE; /* Make room on the stack. */
	SAVE_REGS

    /* we already have trap number in r5 and user data in r6 */
	brlid   r15, mb_exception_dispatcher
	addik   r7, r1, PT_OFFSET /* set third param */

	swi     r3, r1, PT_R3 /*save the result */
	RESTORE_REGS

	addik   r1, r1, STATE_SAVE_SIZE		/* Clean up stack space.  */

	/*return and clear msr[BIP] bit*/
	rtbd    r16, 8
	nop

C_ENTRY(hardware_except_hnd):
 	addik   r1, r1, -STATE_SAVE_SIZE; /* Make room on the stack. */
 	SAVE_REGS

	/* disable interupts*/
	mfs	r12, rmsr
	andni r12, r12, 0x2
	mts	rmsr, r12


	/* set first param (number of exception)*/
	mfs	    r5, resr
	andi    r5, r5, 0x1f

	brlid   r15, mb_hwtrap_dispatcher
	addik   r7, r1, PT_OFFSET /* set third param (pt pointer) */

	/* enable interupts */
	mfs	r12, rmsr
	ori	r12, r12, 0x2
	mts	rmsr, r12

	RESTORE_REGS
 	addik   r1, r1, STATE_SAVE_SIZE; /* Clean up stack space.  */

	rted    r17, 0
	nop

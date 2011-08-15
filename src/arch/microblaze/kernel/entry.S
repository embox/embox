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

	.section ".text"

//XXX Microblaze not suport nested irq
C_ENTRY(_interrupt_handler):
	SAVE_REGS
	/* set third param */
	addik   r7, r1, STATE_SAVE_SIZE

	brlid	r15, interrupt_handler
	 addik	r1, r1, -STATE_SAVE_SIZE; /* Make room on the stack. */

	addik	r1, r1, STATE_SAVE_SIZE		/* Clean up stack space.  */

	RESTORE_REGS

	rtid	 r14, 0 /*return and enable interrupts*/
	nop

C_ENTRY(_user_exception):
C_ENTRY(_debug_exception):
	SAVE_REGS
	/* set third param */
	addik   r7, r1, STATE_SAVE_SIZE
/* we already have trap number in r5 and user data in r6 */
	brlid   r15, mb_exception_dispatcher
	 addik   r1, r1, -STATE_SAVE_SIZE; /* Make room on the stack. */

	addik   r1, r1, STATE_SAVE_SIZE		/* Clean up stack space.  */

    /* the result */
	swi	r3, r1, -PT_R3;

	RESTORE_REGS

	/*return and clear msr[BIP] bit*/
	rtbd    r16,8
	nop

C_ENTRY(_hw_exception_handler):
	SAVE_REGS


	/* set first param (number of exception)*/
	mfs	    r5, resr
	andi    r5, r5, 0x1f

	/* set third param (pt pointer) */
	addi   r7, r1, -PT_R17

	brlid   r15, mb_hwtrap_dispatcher
 	addi   r1, r1, -STATE_SAVE_SIZE; /* Make room on the stack. */

 	addi   r1, r1, STATE_SAVE_SIZE; /* Clean up stack space.  */


	RESTORE_REGS

	rted    r17, 0
	nop

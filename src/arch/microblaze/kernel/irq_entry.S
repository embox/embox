/**
 * @file
 * @brief Low-level interrupt handler
 *
 * @date 25.11.09
 * @author Anton Bondarev
 * @author Ilia Vaprol
 */

#include <asm/linkage.h>
#include <asm/ptrace.h>
#include <asm/regs.h>

	.section .text

C_ENTRY(irq_hnd):
	addik   r1, r1, -STATE_SAVE_SIZE; /* Make room on the stack. */
	SAVE_REGS

	brlid   r15, irq_handler
	addik   r7, r1, PT_OFFSET  /* set third param */

	RESTORE_REGS
	addik   r1, r1, STATE_SAVE_SIZE	/* Clean up stack space.  */

	rtid    r14, 0 /*return and enable interrupts*/
	nop

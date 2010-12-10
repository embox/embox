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

	.section ".trap_table"
	.balign 0x8
C_ENTRY(_start):
	brai boot_start
	.balign 0x8
	brai _user_exception
	.balign 0x8
	brai _interrupt_handler
	.balign 0x8
	brai _debug_exception
	.balign 0x8
	brai _hw_exception_handler


	.section ".text"

C_ENTRY(_hardreset):
	brai boot_start
	nop
	nop
//XXX Microblaze not suport nested irq
C_ENTRY(_interrupt_handler):
	addik	r1, r1, -STATE_SAVE_SIZE; /* Make room on the stack. */
	SAVE_REGS

	brlid	r15, interrupt_handler
	nop

	RESTORE_REGS

	rtid	 r14, 0 /*return and enable interrupts*/
	addik	r1, r1, STATE_SAVE_SIZE		/* Clean up stack space.  */


C_ENTRY(_user_exception):
C_ENTRY(_debug_exception):
#if 0
	addik   r1, r1, -STATE_SAVE_SIZE; /* Make room on the stack. */
	SAVE_REGS

	swi     r17, r1, PT_PC  /* save pc for returning */
	brlid   r15, mb_exception_dispatcher
	/* set third param */
	addik   r7, r1, STATE_SAVE_SIZE

	RESTORE_REGS

	/*return and clear msr[BIP] bit*/
	rtbd    r16,8
	addik   r1, r1, STATE_SAVE_SIZE		/* Clean up stack space.  */

#else

	addi	r1, r1, -4
	swi	r2, r1, 0
	addi	r1, r1, -4
	swi	r3, r1, 0
	addi	r1, r1, -4
	swi	r4, r1, 0
	addi	r1, r1, -4
	swi	r5, r1, 0
	addi	r1, r1, -4
	swi	r6, r1, 0
	addi	r1, r1, -4
	swi	r7, r1, 0
	addi	r1, r1, -4
	swi	r8, r1, 0
	addi	r1, r1, -4
	swi	r9, r1, 0
	addi	r1, r1, -4
	swi	r10, r1, 0
	addi	r1, r1, -4
	swi	r11, r1, 0
	addi	r1, r1, -4
	swi	r12, r1, 0
	addi	r1, r1, -4
	swi	r13, r1, 0
	addi	r1, r1, -4
	swi	r14, r1, 0
	addi	r1, r1, -4
	swi	r15, r1, 0
	addi	r1, r1, -4
	swi	r16, r1, 0
	addi	r1, r1, -4
	swi	r17, r1, 0
	addi	r1, r1, -4
	swi	r18, r1, 0
	addi	r1, r1, -4
	swi	r19, r1, 0
	addi	r1, r1, -4
	swi	r20, r1, 0
	addi	r1, r1, -4
	swi	r21, r1, 0
	addi	r1, r1, -4
	swi	r22, r1, 0
	addi	r1, r1, -4
	swi	r23, r1, 0
	addi	r1, r1, -4
	swi	r24, r1, 0
	addi	r1, r1, -4
	swi	r25, r1, 0
	addi	r1, r1, -4
	swi	r26, r1, 0
	addi	r1, r1, -4
	swi	r27, r1, 0
	addi	r1, r1, -4
	swi	r28, r1, 0
	addi	r1, r1, -4
	swi	r29, r1, 0
	addi	r1, r1, -4
	swi	r30, r1, 0
	addi	r1, r1, -4
	swi	r31, r1, 0
	/* set third param */
	addi	r7, r1, 31*4
	brlid	r15, mb_exception_dispatcher
	nop
	nop
	lwi	r31, r1, 0
	addi	r1, r1, 4
	lwi	r30, r1, 0
	addi	r1, r1, 4
	lwi	r29, r1, 0
	addi	r1, r1, 4
	lwi	r28, r1, 0
	addi	r1, r1, 4
	lwi	r27, r1, 0
	addi	r1, r1, 4
	lwi	r26, r1, 0
	addi	r1, r1, 4
	lwi	r25, r1, 0
	addi	r1, r1, 4
	lwi	r24, r1, 0
	addi	r1, r1, 4
	lwi	r23, r1, 0
	addi	r1, r1, 4
	lwi	r22, r1, 0
	addi	r1, r1, 4
	lwi	r21, r1, 0
	addi	r1, r1, 4
	lwi	r20, r1, 0
	addi	r1, r1, 4
	lwi	r19, r1, 0
	addi	r1, r1, 4
	lwi	r18, r1, 0
	addi	r1, r1, 4
	lwi	r17, r1, 0
	addi	r1, r1, 4
	lwi	r16, r1, 0
	addi	r1, r1, 4
	lwi	r15, r1, 0
	addi	r1, r1, 4
	lwi	r14, r1, 0
	addi	r1, r1, 4
	lwi	r13, r1, 0
	addi	r1, r1, 4
	lwi	r12, r1, 0
	addi	r1, r1, 4
	lwi	r11, r1, 0
	addi	r1, r1, 4
	lwi	r10, r1, 0
	addi	r1, r1, 4
	lwi	r9, r1, 0
	addi	r1, r1, 4
	lwi	r8, r1, 0
	addi	r1, r1, 4
	lwi	r7, r1, 0
	addi	r1, r1, 4
	lwi	r6, r1, 0
	addi	r1, r1, 4
	lwi	r5, r1, 0
	addi	r1, r1, 4
	lwi	r4, r1, 0
	addi	r1, r1, 4
	lwi	r3, r1, 0
	addi	r1, r1, 4
	lwi	r2, r1, 0
	addi	r1, r1, 4


///chush we set msr[BIP] here? than clear it by rtbd:)
		msrset	r0, 8
	nop
	nop
	nop

	rtbd r16,8
	nop
	nop

#endif


C_ENTRY(_hw_exception_handler):
#if 0
	addik   r1, r1, -STATE_SAVE_SIZE; /* Make room on the stack. */
	SAVE_REGS
	swi     r17, r1, PT_PC  /* save pc for returning */

	/* set first param*/
	mfs	    r5, resr
	andi    r5, r5, 0x1f
	brlid   r15, mb_hwtrap_dispatcher
	/* set third param */
	addik   r7, r1, STATE_SAVE_SIZE

	RESTORE_REGS
	/* restore pc for returning pc could be change by dispatcer */
	lwi     r17, r1, PT_PC

	rted    r17, 0
	addik	r1, r1, STATE_SAVE_SIZE		/* Clean up stack space.  */
#else
	addi	r1, r1, -8
	swi	r2, r1, 0
	addi	r1, r1, -4
	swi	r3, r1, 0
	addi	r1, r1, -4
	swi	r4, r1, 0
	addi	r1, r1, -4
	swi	r5, r1, 0
	addi	r1, r1, -4
	swi	r6, r1, 0
	addi	r1, r1, -4
	swi	r7, r1, 0
	addi	r1, r1, -4
	swi	r8, r1, 0
	addi	r1, r1, -4
	swi	r9, r1, 0
	addi	r1, r1, -4
	swi	r10, r1, 0
	addi	r1, r1, -4
	swi	r11, r1, 0
	addi	r1, r1, -4
	swi	r12, r1, 0
	addi	r1, r1, -4
	swi	r13, r1, 0
	addi	r1, r1, -4
	swi	r14, r1, 0
	addi	r1, r1, -4
	swi	r15, r1, 0
	addi	r1, r1, -4
	swi	r16, r1, 0
	addi	r1, r1, -4
	swi	r17, r1, 0
	addi	r1, r1, -4
	swi	r18, r1, 0
	addi	r1, r1, -4
	swi	r19, r1, 0
	addi	r1, r1, -4
	swi	r20, r1, 0
	addi	r1, r1, -4
	swi	r21, r1, 0
	addi	r1, r1, -4
	swi	r22, r1, 0
	addi	r1, r1, -4
	swi	r23, r1, 0
	addi	r1, r1, -4
	swi	r24, r1, 0
	addi	r1, r1, -4
	swi	r25, r1, 0
	addi	r1, r1, -4
	swi	r26, r1, 0
	addi	r1, r1, -4
	swi	r27, r1, 0
	addi	r1, r1, -4
	swi	r28, r1, 0
	addi	r1, r1, -4
	swi	r29, r1, 0
	addi	r1, r1, -4
	swi	r30, r1, 0
	addi	r1, r1, -4
	swi	r31, r1, 0
	/* set first param*/
	mfs		r5, resr
	andi	r5, r5, 0x1f
	/* set third param */
	addi	r7, r1, 0

	brlid	r15, mb_hwtrap_dispatcher
	nop
	nop
	lwi	r31, r1, 0
	addi	r1, r1, 4
	lwi	r30, r1, 0
	addi	r1, r1, 4
	lwi	r29, r1, 0
	addi	r1, r1, 4
	lwi	r28, r1, 0
	addi	r1, r1, 4
	lwi	r27, r1, 0
	addi	r1, r1, 4
	lwi	r26, r1, 0
	addi	r1, r1, 4
	lwi	r25, r1, 0
	addi	r1, r1, 4
	lwi	r24, r1, 0
	addi	r1, r1, 4
	lwi	r23, r1, 0
	addi	r1, r1, 4
	lwi	r22, r1, 0
	addi	r1, r1, 4
	lwi	r21, r1, 0
	addi	r1, r1, 4
	lwi	r20, r1, 0
	addi	r1, r1, 4
	lwi	r19, r1, 0
	addi	r1, r1, 4
	lwi	r18, r1, 0
	addi	r1, r1, 4
	lwi	r17, r1, 0
	addi	r1, r1, 4
	lwi	r16, r1, 0
	addi	r1, r1, 4
	lwi	r15, r1, 0
	addi	r1, r1, 4
	lwi	r14, r1, 0
	addi	r1, r1, 4
	lwi	r13, r1, 0
	addi	r1, r1, 4
	lwi	r12, r1, 0
	addi	r1, r1, 4
	lwi	r11, r1, 0
	addi	r1, r1, 4
	lwi	r10, r1, 0
	addi	r1, r1, 4
	lwi	r9, r1, 0
	addi	r1, r1, 4
	lwi	r8, r1, 0
	addi	r1, r1, 4
	lwi	r7, r1, 0
	addi	r1, r1, 4
	lwi	r6, r1, 0
	addi	r1, r1, 4
	lwi	r5, r1, 0
	addi	r1, r1, 4
	lwi	r4, r1, 0
	addi	r1, r1, 4
	lwi	r3, r1, 0
	addi	r1, r1, 4
	lwi	r2, r1, 0
	addi	r1, r1, 8

	rted r17, 0
	nop
#endif


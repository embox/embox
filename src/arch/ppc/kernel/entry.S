/**
 * @file
 * @brief Low-level interrupt handler
 *
 * @date 12.11.12
 * @author Ilia Vaprol
 */

#include <asm/entry.h>
#include <asm/regs.h>

.globl ext_intr_hnd
ext_intr_hnd:
	SAVE_ALL
	bl interrupt_handle
	RESTORE_ALL
	rfi

.globl decr_hnd
decr_hnd:
	SAVE_ALL            /* save all registers */

	mftsr    r3         /* clear TSR[DIS] */
	lis      r4, TSR_DIS@h
	ori      r4, r4, TSR_DIS@l
	andc     r3, r3, r4
	mttsr    r3

	bl interrupt_handle /* handle interrupt */

	RESTORE_ALL         /* restore all registers */
	rfi                 /* return from interrupt */

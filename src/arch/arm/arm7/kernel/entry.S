/**
 * @file
 * @brief Low-level interrupt handler
 *
 * @date 23.07.10
 * @author Anton Kozlov
 */

#include <asm/modes.h>

.text
.align 4
.global	swi_handler
.global	irq_handler
.global irq_def_handler

irq_def_handler:
	b irq_def_handler

irq_handler:
	sub	LR, LR, #4

	/* storing LP */
	stmfd	sp!, {lr}
	/*storing SPSR and valuable regs*/
	mrs		LR, SPSR
	stmfd	sp!, {r0-r12, lr}

	msr CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT /* going to SYS mode, still disabling interrupts */
	stmfd sp!, {lr} /* stoirqring address of returning in interrupted program flow */

	bl interrupt_handle

	ldmfd sp!, {lr}
	/* now interrupted program is unaffected, interrupt serviced, correctly exiting handler */
	msr CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT /*disable interrupts, restoring state better uninterrupted */

	/*interrupts are disabled, so restoring goes OK */
	ldmfd	sp!, {r0-r12, lr}
	msr		SPSR, LR
	/*regs & SPSR on theirs places, as before handler */
	/*returning from interrupt handler, CPSR <- SPSR */
	ldmfd       sp!, {pc}^
	/*continue from place, where interrupt raised */

	/*What a Terrible Failure? */
	b die

swi_handler:
	/*unimplemented */
die:
	b die

/**
 * @file
 * @brief Low-level interrupt handler
 *
 * @date 23.07.10
 * @author Anton Kozlov
 */

#include <asm/modes.h>

.data
this_spsr_lr:
	.word 0x0
	.word 0x0

.text
.align 4
.global	swi_handler
.global	irq_handler

irq_handler:
	sub	LR, LR, #4

	stmfd	sp!, {r0, r1}
	ldr	r0, =this_spsr_lr
	mrs	r1, SPSR
	stmia	r0, {r1, lr}
	ldmfd	sp!, {r0, r1}

	msr	CPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT
	stmfd	sp!, {r0-r3, lr}

	ldr	r0, =this_spsr_lr
	ldmia	r0, {r1, r2}
	stmfd	sp!,{r1, r2}
	stmfd	sp!,{r0}

	ldr	lr, =irq_leave
	msr	CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT

	msr	SPSR_c, #ARM_MODE_SYS | I_BIT | F_BIT

	ldr	r0, =interrupt_handle
	stmfd	sp!, {r0}
	ldmfd   sp!, {pc}^

irq_leave:
	ldmfd	sp!, {r0}
	ldmfd	sp!, {r1, r2}
	stmia	r0, {r1, r2}

	ldmfd	sp!, {r0-r3, lr}
	msr	CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT

	stmfd	sp!, {r0, r1}
	ldr	r0, =this_spsr_lr
	ldmia	r0, {r1, lr}
	msr	SPSR_c, r1

	ldmfd	sp!, {r0, r1}

	stmfd	sp!, {lr}
	ldmfd   sp!, {pc}^

	/*What a Terrible Failure? */
	b die

swi_handler:

	/* storing LP */
	stmfd	sp!, {lr}
	/*storing SPSR and valuable regs*/
	mrs		LR, SPSR
	stmfd	sp!, {r0-r12, lr}

	stmfd sp!, {lr} /* stoirqring address of returning in interrupted program flow */

	bl swi_handle

	ldmfd sp!, {lr}
	/* now interrupted program is unaffected, interrupt serviced, correctly exiting handler */

	/*interrupts are disabled, so restoring goes OK */
	ldmfd	sp!, {r0-r12, lr}
	msr		SPSR, LR
	/*regs & SPSR on theirs places, as before handler */
	/*returning from interrupt handler, CPSR <- SPSR */
	ldmfd       sp!, {pc}^
	/*continue from place, where interrupt raised */

die:
	b die

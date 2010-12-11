/**
 * @file
 * @brief Low-level interrupt handler
 *
 * @date 23.07.10
 * @author Anton Kozlov
 */
#if 1
/* TODO this constant defines AIC registers map interrupt controller move it from hear*/
#define AT91C_AIC_ISR		0xfffff108
#define AT91C_AIC_EOICR	0xfffff130
#define AT91C_AIC_IVR		0xfffff100
#endif

#include <asm/modes.h>

.text
.align 4
.global	SWI_handler
.global	IRQ_handler
.global IRQ_def_handler

IRQ_def_handler :
	b IRQ_def_handler

IRQ_handler:
	sub	LR, LR, #4

	/* storing LP */
	stmfd	sp!, {lr}
	/*storing SPSR and valuable regs*/
	mrs		LR, SPSR
	stmfd	sp!, {r0-r12, lr}

#ifdef CONFIG_MEASURE
	bl measure_start
	bl measure_stop
	bl measure_irq_measure_overhead
	bl measure_start
#endif

	/*reading IVR, making AIC knowledge of interrupt is taken in account */
	ldr	r0, =AT91C_AIC_IVR
	ldr	r2, [r0]
	/*getting number of source interrupt */
	ldr	r1, =AT91C_AIC_ISR
	ldr	r0, [r1]
	ldr	r1, =0x1f
	and	r0, r1

	/*telling AIC "we've done"*/
	ldr	r1, =AT91C_AIC_EOICR
	str	r0, [r1]

	/*going to higher level handler, passing first arg number of source interrupt */
	mov	r1, #0
	msr CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT /* going to SVC mode, still disabling interrupts */
	stmfd sp!, {lr} /* stoirqring address of returning in interrupted program flow */

#ifdef CONFIG_MEASURE
	stmfd sp!, {r0}
	bl measure_stop
	bl measure_irq_head_process
	bl measure_start
	ldmfd sp, {r0}
#endif

#ifdef CONFIG_NESTED_INTERRUPTS
	msr CPSR_c, #ARM_MODE_SVC /*ready to enable interrupt and go to dispatcher */
#endif

	bl irq_dispatch

#ifdef CONFIG_NESTED_INTERRUPTS
	msr CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT /* need to clean up after dispatcher */
#endif

/*measure irq dispatch*/
#ifdef CONFIG_MEASURE
	bl	measure_stop
	mov r1, r0
	ldmfd sp!, {r0}
	bl measure_irq_process
	bl measure_start
#endif

	ldmfd sp!, {lr}
	/* now interrupted program is unaffected, interrupt serviced, correctly exiting handler */
	msr CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT /*disable interrupts, restoring state better uninterrupted */

#ifdef CONFIG_MEASURE
	bl measure_stop
	bl measure_irq_tail_process
#endif

	/*interrupts are disabled, so restoring goes OK */
	ldmfd	sp!, {r0-r12, lr}
	msr		SPSR, LR
	/*regs & SPSR on theirs places, as before handler */

	/*returning from interrupt handler, CPSR <- SPSR */
	ldmfd       sp!, {pc}^
	/*continue from place, where interrupt raised */

	/*What a Terrible Failure? */
	b die

SWI_handler:
	/*unimplemented */
die:
	b die

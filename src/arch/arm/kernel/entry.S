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
	nop
	sub	LR, LR, #4

	/* storing LP */
	stmfd	sp!, {lr}
	/*storing SPSR and valuable regs*/
	mrs		LR, SPSR
	stmfd	sp!, {r0-r12, lr}

	/*reading IVR, making AIC knowledge of interrupt is taken in account */
	ldr	r0, =AT91C_AIC_IVR
	ldr	r2, [r0]
	/*getting number of source interrupt */
	ldr	r1, =AT91C_AIC_ISR
	ldr	r0, [r1]
	ldr	r1, =0x1f
	and	r0, r1

	/*going to higher level handler, passing first arg number of source interrupt */
	mov	r1, #0

	msr CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT /* going to SVC mode, still disabling interrupts */
	stmfd sp!, {lr} /* storing address of returning in interrupted program flow */

	msr CPSR_c, #ARM_MODE_SVC /*ready to enable interrupt and go to dispatcher */
	bl irq_dispatch

	msr CPSR_c, #ARM_MODE_SVC | I_BIT | F_BIT /* need to clean up after dispatcher */
	ldmfd sp!, {lr}

	/* now interrupted program is unaffected, interrupt serviced, correctly exiting handler */
	msr CPSR_c, #ARM_MODE_IRQ | I_BIT | F_BIT /*disable interrupts, restoring state better uninterrupted */

	/*telling AIC "we've done"*/
	ldr	r1, =AT91C_AIC_EOICR
	str	r0, [r1]

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
	mrs	r2, SPSR
	push	{r0-r4, LR}

	mov	LR, PC
	ldr	PC, =softirq_dispatch

	pop	{r0-r4,	LR}

	msr	CPSR, r2
	mov	PC, LR

die:
	b die

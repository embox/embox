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
	/*storing SPSR */
	mrs		LR, SPSR
	push	{LR}
	/*storing valuable regs */
	push	{r0-r12}

	/*reading IVR, making AIC knowledge of interrupt is taken in account */
	ldr	r0, =AT91C_AIC_IVR
	ldr	r2, [r0]
	/*getting number of source interrupt */
	ldr	r1, =AT91C_AIC_ISR
	ldr	r0, [r1]
	ldr	r4, =0x1f
	and	r0, r4

	/*going to higher level handler, passing first arg number of source interrupt */
	mov	r1, #0
	msr CPSR, #ARM_MODE_SYS /* going to SYS mode, allowing interrupts */
	bl irq_dispatch

	/*handling of returning from handler */
	msr CPSR, #ARM_MODE_IRQ | I_BIT | F_BIT /*to IRQ mode again, disable interrupts */

	/*telling AIC "we've done"*/
	ldr	r1, =AT91C_AIC_EOICR
	str	r0, [r1]

	/*interrupts are disabled, so restoring goes OK */
	pop		{r0-r12}
	pop		{LR}
	msr		SPSR, LR
	/*regs & SPSR on theirs places, as before handler */

	/*returning from interrupt handler, CPSR <- SPSR */
	ldmia       sp!, {pc}^ /*almost imperiative deduced*/
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

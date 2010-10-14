/**
 * @file
 *
 * @brief Low-level interrupt handler
 *
 * @author Anton Kozlov
 */
/* TODO this constant defines AIC registers map interrupt controller move it from hear*/
#define AIC_ISR		0xfffff108
#define AIC_EOICR	0xfffff130
#define AIC_IVR		0xfffff100

.text
.align 4
.global	SWI_handler
.global	IRQ_handler
.global IRQ_def_handler
/* why in special section */
/*.section .entry_text, "x" */

IRQ_def_handler :
	b IRQ_def_handler

IRQ_handler:
	nop
	sub	LR, LR, #4
	push	{r0-r12, r14}
	mrs		r0, SPSR
	push	{r0}

	ldr	r0, =AIC_IVR
	ldr	r2, [r0]
	ldr	r1, =AIC_ISR
	ldr	r0, [r1]
	ldr	r4, =0x1f
	and	r0, r4

	mov	r1, #0
	mov	LR, PC
	ldr	PC, =irq_dispatch


	ldr	r1, =AIC_EOICR
	str	r0, [r1]

	pop		{r0}
	msr		CPSR, r0

	pop		{r0-r12, r14}
	mov	PC, LR

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

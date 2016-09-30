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

irq_handler:
	sub    LR, LR, #4

	/* save temp register*/
	stmfd  sp!, {r0}
	/* storing LP */
	stmfd  sp!, {lr}
	/*storing SPSR and valuable regs*/
	mrs    lr, SPSR
	/* storing SPSR */
	stmfd  sp!, {lr}
	/* pointer to saved on special stack registers */
	mov    r0, sp
	/* return stack pointer to normal value */
	add    sp, sp, #(4 * 3)

	/* return to previous CPU mode with disabled interrupts */
	//orr    lr, lr, #I_BIT | F_BIT
	//msr    CPSR, lr
	msr    CPSR, #ARM_MODE_SYS | I_BIT | F_BIT

	/* now we have previous mode and sp */
	stmfd  sp!, {r1-r12, lr}

	/* restore spsr, lr, tmp reg */
	ldmfd  r0!, {r1} /* spsr */
	ldmfd  r0!, {r2} /* lr */
	ldmfd  r0!, {r3} /* tmp reg */

	stmfd  sp!, {r3}  /* store tmp reg on the stack */
	/* store sp */
	add    r3, sp, #(14 * 4)
	stmfd  sp!, {r3}

	/* store lr */
	stmfd  sp!, {r2}

	/* store spsr */
	stmfd  sp!, {r1}

	mov    r0, sp

	bl     interrupt_handle

	mov    r0, sp
	msr    CPSR, #ARM_MODE_IRQ | I_BIT | F_BIT

	/* restore spsr */
	ldmfd  r0!, {r1}
	msr    SPSR, r1

	/* restore lr */
	ldmfd  r0!, {lr}

	msr    CPSR, #ARM_MODE_SYS | I_BIT | F_BIT
	mov    sp, r0
	/* restore sp */
	ldmfd  sp!, {r3} /* only for balance */
	/* restore tmp reg */
	ldmfd  sp!, {r0}

	ldmfd  sp!, {r1-r12, lr}

	msr    CPSR, #ARM_MODE_IRQ | I_BIT | F_BIT
	stmfd  sp!, {lr}
	/*regs & SPSR on theirs places, as before handler */
	/*returning from interrupt handler, CPSR <- SPSR */
	ldmfd  sp!, {pc}^
	/*continue from place, where interrupt raised */

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


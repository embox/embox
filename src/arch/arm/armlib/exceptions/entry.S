/**
 * @file
 * @brief Low-level interrupt handler
 *
 * @date 23.07.10
 * @author Anton Kozlov
 */

#include <arm/fpu.h>
#include <asm/modes.h>

.text
.align 4
.global	swi_handler
.global	irq_handler

irq_handler:
	sub    LR, LR, #4

	/* save temp register*/
	stmfd  sp!, {r0, r1}
	/* storing LP */
	stmfd  sp!, {lr}
	/*storing SPSR and valuable regs*/
	mrs    lr, SPSR
	/* storing SPSR */
	stmfd  sp!, {lr}
	/* pointer to saved on special stack registers */
	mov    r0, sp
	/* return stack pointer to normal value */
	add    sp, sp, #(4 * (4))

	/* return to previous CPU mode with disabled interrupts */
	msr    CPSR, #ARM_MODE_SYS | I_BIT | F_BIT
	/* now we have previous mode and sp */

	/* restore spsr, lr, tmp reg */
	ldmfd  r0!, {r1} /* spsr */
	stmfd  sp!, {r1} /* spsr */
	ldmfd  r0!, {r1} /* lr */
	stmfd  sp!, {r1} /* lr */

	sub    sp, sp, #8 /* 2 tmp reg * 4 */

	stmfd  sp!, {r2-r12}

	add    sp, sp, #(44 + 8) /* 11 reg + 2 tmp reg * 4 */

	ldmfd  r0, {r3, r4} /* tmp reg */
	stmfd  sp!, {r3, r4}    /* store tmp reg on the stack */
	sub    sp, sp, #44      /* 11 * 4 */

	add    r0, sp, #(52 + 8) /* top of pt_regs */

	stmfd  sp!, {r0, lr}

	mov    r0, sp

	ARM_FPU_CONTEXT_SAVE_DEC(r1, sp)

	bl     interrupt_handle

	msr    CPSR, #ARM_MODE_SYS | I_BIT | F_BIT

	ARM_FPU_CONTEXT_LOAD_DEC(r1, sp)

	ldmfd  sp!, {r0, lr}
	ldmfd  sp!, {r2-r12}

	mov    sp, r0

	msr    CPSR, #ARM_MODE_IRQ | I_BIT | F_BIT
	sub    r0, r0, #8

	ldmfd  r0!, {r1} /* lr */
	mov    lr, r1
	ldmfd  r0!, {r1} /* spsr */
	msr    SPSR, r1

	sub    r0, r0, #16

	ldmfd  r0, {r0, r1} /* tmp reg */
	stmfd sp!, {lr}

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


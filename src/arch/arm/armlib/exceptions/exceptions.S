/**
 * @file
 *
 * @date May 24, 2018
 * @author Anton Bondarev
 */

#include <asm/modes.h>
#include <arm/fpu.h>
#include <asm/cp15.h>

.text
.align 4

.global	undef_handler
.global	data_abt_handler
.global	prefetch_abt_handler
.global	fiq_handler
.global	prefetch_abt_handler
.global arm_unresolvable_exception

undef_handler:
#ifdef __ARM_NEON__
	sub r14, r14, #4

	stmfd sp!, {r0-r2} /* Save temp regs */
	mov r2, r14
	mrs r1, SPSR
	mov r0, sp         /* Pointer to saved regs */
	add sp, sp, #12    /* Return SP to previous state */

	msr CPSR, #ARM_MODE_SYS | I_BIT | F_BIT
	stmfd sp!, {r1}    /* Store arguments for RFE instruction */
	stmfd sp!, {r2}
	ldmfd r0, {r0-r2}  /* Restore temp regs */

	mov r0, sp		/* Pointer to registers */
	stmfd sp!, {r0-r12, r14}
	bl arm_undefined_exception

	ldmfd sp!, {r0-r12, r14}

	rfe sp!
#else /* __ARM_NEON__ */

	sub r14, r14, #4

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

	ARM_FPU_CONTEXT_SAVE_DEC(r1, sp)

	mov    r0, sp

	bl     arm_undefined_exception

	msr    CPSR, #ARM_MODE_SYS | I_BIT | F_BIT

	ARM_FPU_CONTEXT_LOAD_DEC(r1, sp)

	ldmfd  sp!, {r0, lr}
	ldmfd  sp!, {r2-r12}

	mov    sp, r0

	msr    CPSR, #ARM_MODE_UND | I_BIT | F_BIT
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

#endif /* __ARM_NEON__ */


prefetch_abt_handler:
fiq_handler:
	sub r14, r14, #4
	b arm_unresolvable_exception
	sub r14, r14, #8

arm_unresolvable_exception:
	/* This exception may occur if MMU tables or stack pointer
	 * were corrupted, but we still need to print something, i.e.
	 * here we try to avoid infinite recoursive exception */

	/* First, make sure MMU is off before accessing stack */
	mrc p15, 0, r0, c1, c0, 0
	and r0, r0, #~CR_M
	mcr p15, 0, r0, c1, c0, 0
	/* Second, make sure stack pointer has sane value */
	ldr sp, =_stack_top
	/* Now we are ready to process exception itself */
	stmfd sp!, {r0-r12, r14}
	mrs r0, CPSR
	mrs r1, SPSR
	stmfd sp!, {r0, r1}
	mov r0, sp
	b arm_exception_handler

data_abt_handler:
	/* Same as with arm_unresolvable_exception we need
	 * to make sure that RAM access will be safe */
	mrc p15, 0, r0, c1, c0, 0
	and r0, r0, #~CR_M
	mcr p15, 0, r0, c1, c0, 0
	ldr sp, =_stack_top
	stmfd sp!, {r0-r12,r14}
	mov r0, sp
	b arm_data_abort_exception

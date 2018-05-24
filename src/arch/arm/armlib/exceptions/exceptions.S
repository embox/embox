/**
 * @file
 *
 * @date May 24, 2018
 * @author Anton Bondarev
 */

#include <asm/modes.h>

.text
.align 4

.global	undef_handler
.global	prefetch_abt_handler
.global	fiq_handler
.global	prefetch_abt_handler
.global arm_unresolvable_exception
.global data_abt_handler

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
#endif /* __ARM_NEON__ */

prefetch_abt_handler:
fiq_handler:
    sub r14, r14, #4
    b arm_unresolvable_exception
data_abt_handler:
    sub r14, r14, #8

arm_unresolvable_exception:
    nop
    stmfd sp!, {r0-r12, r14}
    mrs r0, CPSR
    mrs r1, SPSR
    stmfd sp!, {r0, r1}
    mov r0, sp
    b arm_exception_handler

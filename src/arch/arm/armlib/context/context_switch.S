/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.10.2012
 */
#include <arm/fpu.h>
	.text
	.global context_switch
context_switch:

	/* Storing and restoring not user, but current set of registers */
	nop

	stmia   r0!, {r0 - r14}
	mrs     r3, cpsr
	stmia   r0!, {r3}
	ARM_FPU_CONTEXT_SAVE_INC(r2, r0)

	mov     r4, r1
	add     r1, r1, #60 /* Now r1 points to stored cpsr */
	ldm     r1, {r3}
	msr     cpsr, r3
	add     r1, r1, #4 /* Now r1 points to stored fpu_data */
	ARM_FPU_CONTEXT_LOAD_INC(r2, r1)
	mov     r1, r4
	ldmia   r1, {r0 - r14}

	nop
	mov	    pc, lr

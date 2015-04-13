/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.10.2012
 */

	.text
	.global context_switch
context_switch:

    /* Storing and restoring not user, but current set of registers */
    nop

    stmia   r0!, {r0 - r14}
    mrs     r3, cpsr
    stmia   r0, {r3}

    add     r1, r1, #60
    ldm     r1, {r3}
    msr     cpsr, r3
    ldmdb   r1, {r0 - r14}

    nop
    mov	    pc, lr


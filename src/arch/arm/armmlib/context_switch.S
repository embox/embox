/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.10.2012
 */

.text
.thumb
.syntax unified
.global context_switch
context_switch:

    stmia   r0, {r0 - r12, r14}
    str	    sp, [r0, #56]
    mrs     r2, CONTROL
    str     r2, [r0, #60]

    ldr     sp, [r1, #56]
    ldr     r2, [r1, #60]
    msr     CONTROL, r2

    add     r0, r1, #56
    ldmdb   r0, {r2 - r12, r14}
    ldr     r0, [r1]
    ldr     r1, [r1, #4]

    mov	    pc, lr


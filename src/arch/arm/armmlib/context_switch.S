/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.10.2012
 */

#include <arm/fpu.h>
.text
.thumb
.syntax unified
.global context_switch
context_switch:

    stmia   r0, {r0 - r12, r14}
    str	    sp, [r0, #56]
    mrs     r2, CONTROL
    str     r2, [r0, #60]
    add     r0, r0, #64
    ARM_FPU_CONTEXT_SAVE_INC(r3, r0)

    mov     r4, r1
    ldr     sp, [r1, #56]
    ldr     r2, [r1, #60]
    msr     CONTROL, r2
    add     r1, r1, #64
    ARM_FPU_CONTEXT_LOAD_INC(r3, r1)
    mov     r1, r4
    ldmia   r1, {r0 - r12, r14}

    mov	    pc, lr


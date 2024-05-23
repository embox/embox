/**
 * @file
 * @brief
 *
 * @author Anton Kozlov
 * @date 25.10.2012
 */

#include <arm/fpu.h>

.text
.thumb
.syntax unified
.global context_switch

context_switch:
#if !defined(__ARM_ARCH_6M__)
	stmia   r0, {r0 - r12, r14}
	str     sp, [r0, #56]
	mrs     r2, CONTROL
	str     r2, [r0, #60]
	add     r0, r0, #64
	save_fpu_inc tmp=r3, stack=r0

	mov     r4, r1
	ldr     sp, [r1, #56]
	ldr     r2, [r1, #60]
	msr     CONTROL, r2
	add     r1, r1, #64
	load_fpu_inc tmp=r3, stack=r1
	mov     r1, r4
	ldmia   r1, {r0 - r12, r14}

	mov     pc, lr
#else
	adds    r0, #8
	stmia   r0!, {r2 - r7}
	mov     r2, r8
	mov     r3, r9
	mov     r4, r10
	mov     r5, r11
	mov     r6, r12
	mov     r7, lr
	stmia   r0!, {r2 - r7}
	mov     r2, sp
	str     r2, [r0]
	mrs     r2, CONTROL
	str     r2, [r0, #4]

	ldr     r2, [r1, #56]
	mov     sp, r2
	ldr     r2, [r1, #60]
	msr     CONTROL, r2
	adds    r1, #32
	ldmia   r1!, {r2 - r7}
	mov     r8, r2
	mov     r9, r3
	mov     r10, r4
	mov     r11, r5
	mov     r12, r6
	mov     lr, r7
	subs    r1, #48
	ldmia   r1!, {r2 - r7}

	mov     pc, lr
#endif

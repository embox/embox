/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

#include <asm/asm.h>

.text
.syntax unified

ENTRY(ptregs_jmp)
#if !defined(__ARM_ARCH_6M__)
	add     r0, #4
	ldmia   r0, {r1 - r12, r14}
	ldr     sp, [r0, #52]
	add     sp, sp, #68
	ldr     r0, [r0, #-4]
	mov     pc, lr

#else

	ldr     r1, [r0, #56]
	mov     sp, r1

	adds    r0, #32
	ldmia   r0!, {r2 - r7}
	mov     r8, r2
	mov     r9, r3
	mov     r10, r4
	mov     r11, r5
	mov     r12, r6
	mov     r14, r7
	subs    r0, #52
	ldmia   r0!, {r1 - r7}
	subs    r0, #28

	add     sp, sp, #68
	subs    r0, #4
	ldr     r0, [r0]
	mov     pc, lr
#endif
END(ptregs_jmp)

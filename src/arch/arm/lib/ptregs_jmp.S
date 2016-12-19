/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

.text
.global ptregs_jmp
.syntax unified

ptregs_jmp:
	add     r0, #4
	ldmia   r0, {r1 - r12, r14}
	ldr     sp, [r0, #52]; \
	add     sp, sp, #68
	ldr     r0, [r0, #-4]
	mov     pc, lr


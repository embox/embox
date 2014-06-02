/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

	.text
	.global ptregs_jmp

ptregs_jmp:
	mov	sp, r0
	ldmia	sp, {r0-r14}
	add	sp, sp, #68
	mov	pc, lr


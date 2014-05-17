/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.05.2014
 */

	.text
	.global vfork
	.global vfork_leave

vfork:
	sub 	sp, sp, #68
	stmia 	sp, {r0-r14}

	mov	r0, sp

	bl vfork_body

vfork_leave:
	mov	sp, r0
	ldmia	sp, {r0-r14}
	add	sp, sp, #68
	mov	pc, lr


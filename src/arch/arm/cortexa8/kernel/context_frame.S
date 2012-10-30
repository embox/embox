/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.10.2012
 */

.global frame_tramp
frame_tramp:
	stmfd	sp!,{r0-r3}
	ldr	r1, [sp, #16]
	mrs	r2, cpsr
	stmfd	sp!, {r2}

	blx	r1

	ldmfd	sp!, {r2}
	msr	cpsr, r2

	ldmfd	sp!,{r0-r3}

	add	sp, sp, #4

	ldmfd	sp!,{lr}

	mov	pc, lr



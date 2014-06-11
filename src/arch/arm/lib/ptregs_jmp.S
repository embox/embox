/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

	.text
	.global ptregs_jmp

#if defined(__thumb2__) || defined(__thumb__)

ptregs_jmp:
	/* NYI */
	b ptregs_jmp

#else /* Arm */

ptregs_jmp:
	mov	sp, r0
	ldmia	sp, {r0-r14}
	add	sp, sp, #68
	mov	pc, lr

#endif


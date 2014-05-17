/**
 * @file
 *
 * @data May 7, 2014
 * @author Anton Bondarev
 */


.global cpu_stack_move
.type cpu_stack_move, %function

.text
cpu_stack_move:
	push {r0, r1, r2, lr}

	sub r2, r1, sp

	sub r0, r0, r2
	sub r1, r1, r2

	bl  memcpy
	pop {r0, r1, r2, lr}

	sub r2, r1, r0

	sub sp, sp, r2
	sub fp, fp, r2

    mov pc, lr

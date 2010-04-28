/**
 * @file
 * @brief Simple routine which provokes all windows to be flushed onto the
 *        stack.
 *
 * @date 13.04.2010
 * @author Eldar Abusalimov
 */

	.text
	.align 4

#include <asm/stack.h>

	.global winflush
winflush:

	mov CONFIG_NWINDOWS - 1, %g1
1:	subcc %g1, 1, %g1
 	bg 1b
	 save %sp, -STACK_FRAME_SZ, %sp

	mov CONFIG_NWINDOWS - 1, %g1
1:	subcc %g1, 1, %g1
 	bg 1b
	 restore

	retl
	 nop

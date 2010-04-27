/**
 * @file
 * @brief TODO
 *
 * @date 13.04.2010
 * @author Eldar Abusalimov
 */

	.text
	.align 4

	.global winflush
winflush:
	/* Provoke all windows to be flushed onto the stack. */
	mov CONFIG_NWINDOWS - 1, %g1
1:	subcc %g1, 1, %g1
 	bg 1b
	 save %sp, -0x60, %sp
	mov CONFIG_NWINDOWS - 1, %g1
1:	subcc %g1, 1, %g1
 	bg 1b
	 restore

	retl
	 nop

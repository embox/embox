/**
 * \file window_save.S
 *
 * \date May 15, 2009
 * \author afomin
 */

#include "leon_config.h"

#define SA(x)		(((x)+7)&(~0x07))	/* rounds "x" up to doubleword boundary */
#define TRAPFRAME 	((16+7+1+8)*4)		/* trap frame size (see comment below) */

 	.section	".text"
	.align	4
	.global window_save

window_save:
	mov		%psr, %l4
	and		%l4, 0x1F, %l4
	mov		%wim, %l7
	srl		%l7, %l4, %l7
	and		%l7, 0x1, %l7
	cmp		%l7, 0x1			! test WIM for possible window overflow

	bne		irq_overflow_test_ok
	 nop

		! handle window overflow
		mov  %wim, %l5			! Calculate next WIM
		mov  %g1, %l7
		srl  %l5, 1, %g1
		sll  %l5, CORE_NWINDOWS-1 , %l4
		or   %l4, %g1, %g1

		save					! Get into window to be saved.
		mov  %g1, %wim
		std	%l0, [%sp + 0x00];
		std	%l2, [%sp + 0x08];
		std	%l4, [%sp + 0x10];
		std	%l6, [%sp + 0x18];
		std	%i0, [%sp + 0x20];
		std	%i2, [%sp + 0x28];
		std	%i4, [%sp + 0x30];
		std	%i6, [%sp + 0x38];
		restore					! Go back to trap window.
		mov		%l7, %g1

	irq_overflow_test_ok:
	add		%fp, -SA(TRAPFRAME), %sp

	mov	%y, %l6
	st	%l6, [%sp + 0x60];
	st	%g1, [%sp + 0x64];
	std	%g2, [%sp + 0x68];
	std	%g4, [%sp + 0x70];
	std	%g6, [%sp + 0x78];

	srl		%l3, 4, %o0
	and 	%o0, 0xFF, %o0		! pass TT as parameter

	mov		%psr, %l6
	or		%l6, 0x20, %l6	! set PSR.ET (enable traps)
	mov		%l6, %psr
	nop; nop; nop

	jmpl %r15 + 8,%g0
	 nop

/**
 * \file window_restore.S
 *
 * \date May 15, 2009
 * \author afomin
 */

 #include "leon_config.h"

#define SA(x)		(((x)+7)&(~0x07))	/* rounds "x" up to doubleword boundary */
#define TRAPFRAME 	((16+7+1+8)*4)		/* trap frame size (see comment below) */

 	.section	".text"
	.align	4

	.global window_restore
window_restore:
	mov		%psr, %l6
	andn	%l6, 0x20, %l6	! clear PSR.ET (disable traps)
	mov		%l6, %psr

	ld	[%sp + 0x60], %l6;
	mov	%l6, %y
	ld	[%sp + 0x64], %g1;
	ldd	[%sp + 0x68], %g2;
	ldd	[%sp + 0x70], %g4;
	ldd	[%sp + 0x78], %g6;

	! handle possible window underflow when executing RETT
	mov		%psr, %l4
	and		%l4, 0x1F, %l4		! calculate current CWP
	mov		%wim, %l7
	srl		%l7, 1, %l5
	sll		%l7, CORE_NWINDOWS-1 , %l6
	or		%l5, %l6, %l7
	srl		%l7, %l4, %l7
	and		%l7, 0x1, %l7
	cmp		%l7, 0x1			! test WIM value after RETT for window underflow

	bne		irq_underflow_test_ok
	 nop

		! handle window underflow
		mov		%wim, %l6                  ! Calculate next WIM
		sll		%l6, 1, %l4
		srl		%l6, CORE_NWINDOWS-1, %l5
		or		%l5, %l4, %l5
		mov		%l5, %wim
		nop; nop; nop			! wait for delayed-write

		restore					! get into the window to be restore
		ldd	[%sp + 0x00], %l0;
		ldd	[%sp + 0x08], %l2;
		ldd	[%sp + 0x10], %l4;
		ldd	[%sp + 0x18], %l6;
		ldd	[%sp + 0x20], %i0;
		ldd	[%sp + 0x28], %i2;
		ldd	[%sp + 0x30], %i4;
		ldd	[%sp + 0x38], %i6;
		save					! Get back to the trap window.

	irq_underflow_test_ok:
    ! restore PIL&ICC fields of old PSR (in %l0)
	mov		%psr, %l4
	set		0xF00F00, %l3
	andn	%l4, %l3, %l4
	and		%l0, %l3, %l0
	or		%l4, %l0, %l4
	mov		%l4, %psr
	nop; nop; nop			! wait for PSR delayed-write

	jmpl	%r15, %g0
	 nop

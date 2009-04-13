/*
 * window_overflow_func.S
 *
 *  Created on: Feb 5, 2009
 *      Author: anton
 */
#include "leon_config.h"

   	.section    "text"
	.align  4
	.global window_overflow_func

window_overflow_func:
	mov	%g1, %l7		! Remember previous value of %g1 to restore it later

	mov	%wim, %g1		! Calculate new WIM
	srl	%g1, 1, %l3
	sll	%g1, CORE_NWINDOWS-1 , %g1
	or	%l3, %g1, %g1	! Circular right shifting by 1 bit

	save		! Get into window to be saved
	mov	%g1, %wim	! Write new WIM value that has been just calculated
	std	%l0, [%sp + 0x00];
	std	%l2, [%sp + 0x08];
	std	%l4, [%sp + 0x10];
	std	%l6, [%sp + 0x18];
	std	%i0, [%sp + 0x20];
	std	%i2, [%sp + 0x28];
	std	%i4, [%sp + 0x30];
	std	%i6, [%sp + 0x38];
	restore		! Go back to trap window

	mov	%l7, %g1	! Restore saved %g1

	jmpl %o7 + 8,%g0!jmpl %r15 + 8,%g0 //return if call without save
	nop

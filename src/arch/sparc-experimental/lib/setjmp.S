/**
 * @file
 * @brief TODO
 *
 * @date 31.01.2010
 * @author Eldar Abusalimov
 */

#include <asm/setjmp.h>
#include <asm/winmacro.h>

	.text
	.align 4

	.global setjmp
setjmp:
	st %fp, [%o0 + JB_FP]
	st %i7, [%o0 + JB_I7]
	st %sp, [%o0 + JB_SP]
	st %o7, [%o0 + JB_O7]
	retl
	 mov %g0, %o0

	.global longjmp
longjmp:
	/* Provoke all windows to be flushed onto the stack. */
	mov CONFIG_NWINDOWS - 1, %g1
1:	subcc %g1, 1, %g1
 	bg 1b
	 save
	mov CONFIG_NWINDOWS - 1, %g1
1:	subcc %g1, 1, %g1
 	bg 1b
	 restore

	/* It is guaranteed that window to be entered executing RESTORE is invalid.
	 * Just restore the current window from a stack frame pointed by saved %sp
	 * value, the next window will be restored by window underflow handler from
	 * saved %fp pointed frame and so on. */

	ld [%o0 + JB_SP], %sp

	LOAD_WINDOW(sp)

	ld [%o0 + JB_FP], %fp
	ld [%o0 + JB_I7], %i7

	/* Test the second argument (it will become return value for setjmp). */
	orcc %g0, %o1, %g1
	be,a 1f ! 0 is replaced by 1
	 mov 1, %g1

	/* Return address given by caller of setjmp. */
	ld [%o0 + JB_O7], %o7

1:	retl
	 mov %g1, %o0

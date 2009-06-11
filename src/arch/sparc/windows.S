/**
 * \file windows.S
 *
 * \brief SPARC windows overflow/underflow routines
 *
 * \date Jun 4, 2009
 * \author Eldar Abusalimov
 */

#include "windows.h"
#include "traps.h"


/* Window overflow handling routine. */
		.global window_overflow
window_overflow:

#define g_newwim g1

	/* Remember previous value of %g_newwim to restore it later */
	mov %g_newwim, %t0

	/* Calculate new WIM */
	WIM_SHIFT_RIGHT(t_wim, g_newwim, t1)
	/*
	 * at this line WIM points to the current window
	 * so SAVE wouldn't not cause any trap
	 */
	save	// Get into the window to be saved

	mov %g_newwim, %wim	// Write new WIM value that has been just calculated
	 // do not wait for delayed write

	STORE_WINDOW(sp, 0)

	/*
	 * at this line WIM points to the current window again
	 * so RESTORE wouldn't not cause any trap
	 */
	restore	// Go back to trap window

	/* Restore saved %g_newwim */
	mov %t0, %g_newwim

	jmp %t_pc	// Re-execute SAVE
	 rett %t_npc


/* Window underflow handling routine.  */
		.global  window_underflow
window_underflow:

#define l_newwim t0

	/* Calculate new WIM */
	WIM_SHIFT_LEFT(t_wim, l_newwim, t1)

	mov	%l_newwim, %wim	// Write new WIM value
	 nop; nop; nop	// Wait for WIM delayed writing

	/*
	 * Two restores to get into the window to be restored
	 */
	restore
	restore

	LOAD_WINDOW(sp, 0)

	save
	save	// Get back to trap window

	jmp  %l1	// Re-execute RESTORE
	 rett  %l2


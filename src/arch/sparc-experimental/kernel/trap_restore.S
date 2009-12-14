/**
 * @file
 *
 * @brief Low-level trap post-setup and SPARC windows underflow routines.
 *
 * @details Here is described the algorithm itself. More general information
 * can be found in @link windows.h @endlink docs.
 *
 ** @par Introduction
 *       Will use the following notation:
 *       @n <tt>(T)</tt> - the trap time window,
 *       @n <tt>(K)</tt> - kernel window,
 *       @n <tt>(U)</tt> - user window,
 *       @n <tt>(L)</tt> - the last user window,
 *       @n <tt>(*)</tt> - invalid bit of @em WIM,
 *       @n <tt>(@)</tt> - the window currently pointed by the @em CWP.
 *
 * @par
 *       Orientation of @em WIM register layout used in the examples below:
@verbatim
         +-----+---+---+---+---+---+---+---+---+
   win#: | ... | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
         +-----+---+---+---+---+---+---+---+---+
 <-- RESTORE, RETT                      TRAP, SAVE -->
@endverbatim
 *
 *
 ** @par @em RETT back to user mode with an available window
 *       <em>(WIM & ~(1<<((CWP+1)%N))) != 0</em>
@verbatim
         +-----+---+---+---+---+---+---+---+---+
 before: |  -  | -*| U | L*|@T | - | - | - | - |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       Remove the secondary @em WIM bit (corresponds to the window  that we
 *       will land to when executing @em RETT, (L)).
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  -  | -*| U | L |@T | - | - | - | - |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 *
 ** @par @em RETT back to kernel with an available window
 *       <em>(WIM & ~(1<<((CWP+1)%N))) != 0</em>
@verbatim
         +-----+---+---+---+---+---+---+---+---+
         |  -  | -*| ? | K |@T | - | - | - | - |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       Just do it!
 *
 *
 ** @par @em RESTORE into an invalid window (user/kernel mode)
@verbatim
         +-----+---+---+---+---+---+---+---+---+
 before: |  -  | - | -*| ? |@T | - | - | - | - |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       Restore the necessary window from the user/kernel stack, and circular
 *       shift the invalid bit to the left.
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  -  | -*| ? | ? |@T | - | - | - | - |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 *
 ** @par @em RETT into an invalid window (user/kernel mode)
 *       <em>(WIM & ~(1<<((CWP+1)%N))) == 0</em>
@verbatim
         +-----+---+---+---+---+---+---+---+---+
 before: |  -  | - | - | -*|@T | - | - | - | - |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       Do the same as @em RESTORE window underflow handler explained above.
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  -  | - | -*| ? |@T | - | - | - | - |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 * @sa windows.h
 *
 * @author Eldar Abusalimov
 */

#include <asm/trap_setup.h>
#include <asm/windows.h>
#include <asm/regs.h>

	.text
	.align 4

#define g_newwim     g1
#define g_temp       g2

/**
 * Performs some post-trap-handler routines such as
 * TODO calling scheduler, checking for pending signals and
 * checking if the RETT would land us in an invalid window.
 *
 * The code branching us here looks as follows:
 *
 *	ba trap_setup_end
 *	 rd %wim, %t_wim
 *
 */	.global trap_setup_end
trap_setup_end:
	/* Check the mode, load the TRAP_REGS structure saved in trap_begin,
	 * and handle possible window underflow when executing RETT.
	 * TODO scheduler and signals
	 */

	/* First of all, restore some important locals. */
	LOAD_TR_PRIV(sp, t_psr, t_pc, t_npc)

	mov 0x1, %t_twinmask
	sll %t_twinmask, %t_psr, %t_twinmask
	/* Calculate the mask of the window that will be entered after RETT. */
	sll %t_twinmask, 1, %g_temp
	srl %t_twinmask, CORE_NWINDOWS - 1, %t_uwinmask

	/* Check whether we are going to return to kernel or user code... */
	andcc %t_psr, PSR_PS, %g0
	/* ...and branch conditionally. */
	be tse_user
	 or %t_uwinmask, %g_temp, %t_uwinmask

tse_kernel:
	/* Check for window underflow when executing RETT. */
	andncc %t_wim, %t_uwinmask, %g0
	bne tse_finishup_loadregs
	 nop

	/* WIM has got only one bit set, and there are no user windows.
	 * Reset WIM to perform RESTORE safely. */
	wr %g0, %g0, %wim
	 // do not wait
	/* Calculate new WIM. */
	sll %t_uwinmask, 1, %g_temp
	srl %t_uwinmask, CORE_NWINDOWS - 1, %g_newwim
	or  %g_newwim, %g_temp, %g_newwim

	restore	// get into the window to be restored
	wr %g_newwim, %g0, %wim
	 // do not wait
	LOAD_WINDOW(sp, 0)
	ba tse_finishup_loadregs
	 save	// Get back to the trap window

tse_user:
	// TODO scheduler and signals

	rd %wim, %t_wim
	/* Check for window underflow when executing RETT.
	 * Also unmark the last user window (if any). */
	andncc %t_wim, %t_uwinmask, %g_newwim
	bne,a tse_finishup_loadregs
	 wr %g_newwim, %g0, %wim
	  // do not wait

	/* WIM has got only one bit set, and there are no user windows.
	 * Reset WIM to perform RESTORE safely */
	wr %g0, %g0, %wim
	 // do not wait
	/* Calculate new WIM. */
	sll %t_uwinmask, 1, %g_temp
	srl %t_uwinmask, CORE_NWINDOWS - 1, %g_newwim
	or  %g_newwim, %g_temp, %g_newwim

	restore	// get into the window to be restored
	wr %g_newwim, %g0, %wim
	 // do not wait

	USER_STACK_PRECHECK(tse_user_stack_is_corrupt, l0)

	/* Attempt to restore the registers. */
	LOAD_WINDOW(sp, 0)

	save	// Get back to trap window

	USER_STACK_POSTCHECK(tse_user_stack_is_corrupt, t_temp)

tse_finishup_loadregs:
	wr %t_psr, %g0, %psr
	 // do not wait
	/* Restore trap time registers form the stack. */
	LOAD_TR_UNPRIV(sp, g_temp)

	/* see you soon! */
	jmp %t_pc
	 rett %t_npc

#undef g_newwim
#undef g_temp


#define t_newwim     t_ksp
#define t_temp       t_retpc

/**
 * Window underflow handling routine.
 *
 * Assumes that the trap entry point has already done the following:
 *
 *	rd %psr, %t_psr
 *	rd %wim, %t_wim
 *	ba window_underflow
 *	 andcc %t_psr, PSR_PS, %g0
 *
 */	.global window_underflow
window_underflow:
	/* Good news! It is guaranteed that WIM is in single-bit form,
	 * and and we can get the new value using %t_wim. */

	/* Calculate new WIM. */
	sll %t_wim, 1, %t_temp
	srl %t_wim, CORE_NWINDOWS - 1, %t_newwim

	wr %t_newwim, %t_temp, %wim	// Write new WIM value..
	 nop	// ..and wait a bit (see the following instructions)

	/* Check the result of kernel/user mode testing
	 * executed by the trap entry, and branch conditionally.
	[andcc %t_psr, PSR_PS, %g0] */
	bne,a wuf_from_kernel
	 /* for kernel: restore the PSR modified when checking the mode. */
	 wr %t_psr, %g0, %psr
	  // do not wait

wuf_from_user:
	restore	// two restores to get into the window to be loaded
	restore

	USER_STACK_PRECHECK(wuf_user_stack_is_corrupt, l0)

	/* Attempt to restore the registers. */
	LOAD_WINDOW(sp, 0)

	save
	save	// Get back to trap window

	USER_STACK_POSTCHECK(wuf_user_stack_is_corrupt, t_temp)

	/* restore the PSR modified when playing with user stack. */
	wr %t_psr, %g0, %psr
	 nop	// wait a bit (see the following instructions)

	jmp %t_pc	// Re-execute RESTORE
	 rett %t_npc

wuf_from_kernel:
	restore	// two restores to get into the window to be loaded
	restore

	LOAD_WINDOW(sp, 0)

	save
	save	// Get back to trap window

	jmp %t_pc	// Re-execute RESTORE
	 rett %t_npc

wuf_user_stack_is_corrupt:
	/* User wants to have problems.
	 * TODO kill the current process.
	 */
	restore

	wr %t_wim, %g0, %wim
	 // do not wait

	wr %t_psr, %g0, %psr
	 nop	// wait a bit (see the following instructions)

	jmp %t_pc	// Re-execute RESTORE
	 rett %t_npc


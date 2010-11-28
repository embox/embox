/**
 * @file
 * @brief Low-level trap post-setup and SPARC windows underflow routines.
 *
 * @details Here is described the algorithm itself. More general information
 * can be found in @link wim.h @endlink docs.
 *
 ** @par Introduction
 * Will use the following notation:
 *       @n <tt>(T)</tt> - the trap time window,
 *       @n <tt>(K)</tt> - kernel window,
 *       @n <tt>(U)</tt> - user window,
 *       @n <tt>(L)</tt> - the last user window,
 *       @n <tt>(*)</tt> - invalid bit of @em WIM,
 *       @n <tt>(@)</tt> - the window pointed by the @em CWP.
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
 *
 * @sa wim.h
 *
 * @author Eldar Abusalimov
 */

#include <asm/regs.h>
#include <asm/psr.h>
#include <asm/wim.h>
#include <asm/asi.h>
#include <asm/ptrace.h>
#include <asm/winmacro.h>

	.text
	.align 4

/* Trap post-setup. */

/** Used for new WIM calculation and inter-window reference. */
#define g_newwim g1
/** For temporal computations. */
#define g_temp   g2
/** The mask of the last user window (if any). */
#define t_uwinmask   t_retpc

/**
 * Performs some post-trap-handler routines such as
 * TODO calling scheduler, checking for pending signals and
 * checking if the RETT would land us in an invalid window.
 *
 * The code branching us here looks as follows:
@code
	ba trap_setup_end
	 rd %wim, %t_wim
@endcode
 *
 */	.global trap_setup_end
trap_setup_end:
	/* Check the mode, load necessary registers saved in the corresponding
	 * routine, and handle possible window underflow when executing RETT.
	 */

	/* First of all, restore some important locals. */
	LOAD_PT_PRIV(sp, t_psr, t_pc, t_npc)

	mov 0x1, %t_twinmask
	sll %t_twinmask, %t_psr, %t_twinmask
	/* Calculate the mask of the window that will be entered after RETT. */
	sll %t_twinmask, 1, %t_uwinmask
	srl %t_twinmask, CONFIG_NWINDOWS - 1, %g_temp
	or  %t_uwinmask, %g_temp, %t_uwinmask

	/* Check whether we are going to return to kernel or user code... */
	andcc %t_psr, PSR_PS, %g0
	/* ...and branch conditionally. */
	be tse_user
	 /* delay slot: Test for window underflow when executing RETT. */
	 andncc %t_wim, %t_uwinmask, %g0

tse_kernel:
	/* Check the result of window underflow test. */
	be tse_kernel_wuf
	 /* delay slot: restore trap time PSR value. Callee shouldn't touch it. */
	 wr %t_psr, %g0, %psr
	  ! do not wait

tse_finishup:
	/* Restore trap time registers from the stack. */
	LOAD_PT_UNPRIV(sp, g_temp)

	/* see you soon! */
	jmp %t_pc
	 rett %t_npc

tse_kernel_wuf:
	/* Note that PSR has been restored when branching here, and we do not
	 * restore it again at the end, so be careful and do not touch PSR fields
	 * (particularly condition codes) in this routine. */

	/* WIM has got only one bit set, and there are no user windows.
	 * Calculate new WIM (circular left shifting). */
	sll %t_uwinmask, 1, %g_newwim
	srl %t_uwinmask, CONFIG_NWINDOWS - 1, %g_temp
	or %g_newwim, %g_temp, %g_newwim
	// XXX severe bug was here, review other locore part. -- Eldar
	// XORing impure shifted t_uwinmask's results in zero WIM value.
	wr %g_newwim, %g0, %wim
	 ! do not wait

	/* Restore trap time unprivileged registers from the stack. */
	LOAD_PT_UNPRIV(sp, g_temp)

	restore ! get into the window to be restored

	LOAD_WINDOW(sp)

	save ! Get back to the trap window

	jmp %t_pc
	 rett %t_npc

tse_user:
	// TODO scheduler and signals

	rd %wim, %t_wim
	/* Check for window underflow when executing RETT.
	 * Also unmark the last user window (if any). */
	andncc %t_wim, %t_uwinmask, %g_newwim
	be tse_user_wuf
	 wr %g_newwim, %g0, %wim
	  ! do not wait

	ba tse_finishup
	 wr %t_psr, %g0, %psr
	  ! do not wait

tse_user_wuf:
	/* WIM had got only one bit set, and there are no user windows.
	 * Caller has reset WIM for us to perform RESTORE safely. */

	/* Calculate new WIM. */
	sll %t_uwinmask, 1, %g_newwim
	srl %t_uwinmask, CONFIG_NWINDOWS - 1, %g_temp
	or  %g_newwim, %g_temp, %g_newwim

	restore ! get into the window to be restored
	wr %g_newwim, %g0, %wim
	 ! do not wait

	/* Locals are about to be reloaded, so we may trash them.
	 * Attempt to restore the registers. */
	LOAD_USER_WINDOW(sp, tse_user_stack_is_corrupt, l0)

	save ! Get back to trap window

	ba tse_finishup
	 wr %t_psr, %g0, %psr
	  ! do not wait

#undef g_newwim
#undef g_temp

/* Window underflow trap. */

/** Used for new WIM calculation. */
#define t_newwim local
/** For temporal computations. */
#define t_temp   temp

/**
 * Window underflow handling routine.
 *
 * Assumes that the trap entry point has already done the following:
@code
	rd %psr, %t_psr
	ba window_underflow
	 rd %wim, %t_wim
	nop
@endcode
 *
 */	.global window_underflow
window_underflow:
	/* Good news! It is guaranteed that WIM is in single-bit form,
	 * and and we can get the new value using %t_wim. */

	/* Calculate new WIM. */
	sll %t_wim, 1, %t_newwim
	srl %t_wim, CONFIG_NWINDOWS - 1, %t_temp
	wr %t_newwim, %t_temp, %wim
	 ! do not wait

	/* Check the kernel/user mode and branch conditionally. */
	andcc %t_psr, PSR_PS, %g0
	be wuf_from_user
	 /* delay slot: Restore PSR modified when checking the mode. Makes sense
	  * only for kernel, user-mode routine have to restore it later again. */
	 wr %t_psr, %g0, %psr
	  ! do not wait

wuf_from_kernel:
	/* We assume that kernel does not "overrestores", so the window to be
	 * loaded belongs to kernel code and does not need stack checking.
	 */

	! XXX I don't know why, but this delay is really needed. -- Eldar
	nop; ! the same should be inserted in user mode.

	restore ! two restores to get into the window to be loaded
	restore

	LOAD_WINDOW(sp)

	save
	save ! Get back to trap window

	jmp %t_pc
	 rett %t_npc

wuf_from_user:
	nop; ! XXX see above

	restore ! two restores to get into the window to be loaded
	restore

	/* Locals are about to be reloaded, so we may trash them.
	 * Attempt to restore the registers. */
	LOAD_USER_WINDOW(sp, wuf_user_stack_is_corrupt, l0)

	save
	save ! Get back to trap window

	/* restore the PSR modified when playing with user stack. */
	wr %t_psr, %g0, %psr
	 nop ! wait a bit (see the following instructions)

	jmp %t_pc
	 rett %t_npc

wuf_user_stack_is_corrupt:
	/* User wants to have problems.
	 * TODO kill the current process.
	 */
	save
	save ! Get back to trap window

	// TODO
	wr %t_wim, %g0, %wim
	 // do not wait

	wr %t_psr, %g0, %psr
	 nop	// wait a bit (see the following instructions)

	jmp %t_pc	// Re-execute RESTORE
	 rett %t_npc

#undef t_newwim
#undef t_temp

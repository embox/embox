/**
 * @file trap_save.S
 *
 * @brief Low-level trap pre-setup and SPARC windows overflow routines.
 *
 * @details Here is described the algorithm itself. More general information
 * can be found in @link windows.h @endlink docs.
 *
 * Will use the following notation:
 * @li (T) - the trap time window,
 * @li (K) - kernel window,
 * @li (U) - user window,
 * @li (L) - the last user window,
 * @li (*) - invalid bit of WIM,
 * @li (@) - the window currently pointed by the CWP.
 *
 * Orientation of WIM register layout used in the examples below:
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 *   win#: | ... | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *         +-----+---+---+---+---+---+---+---+---+
 * <-- RESTORE, RETT                      TRAP, SAVE -->
@endverbatim
 *
 *
 ** SAVE from user mode into an invalid window
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 * before: |  U  | U | U | L |@T*| U | U | U | U |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 * It's guaranteed that all windows belong to the user.
 * Try to save the next window to the user stack,
 * and circular shift the invalid bit to the right if all is ok.
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 *  after: |  U  | U | U | L |@T | -*| U | U | U |
 *  (ok)   +-----+---+---+---+---+---+---+---+---+
@endverbatim
 * If window saving fails, free all user windows by
 * circular shifting the invalid bit to the left.
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 *  after: |  -  | - | - | -*|@T | - | - | - | - |
 *  (fail) +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 ** TRAP from user mode with an available window
 * (WIM & (1<<CWP)) == 0
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 * before: |  U  | U | U | L |@T | - | -*| U | U |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 * Mark the window we came from (L) as an invalid.
 * This mark is called secondary as opposite to the primary one, which
 * denotes the actually invalid window.
 * The secondary mark will be used later to determine whether there are
 * any user windows on the CPU to deal with.
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 *  after: |  U  | U | U | L*|@T | - | -*| U | U |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 ** TRAP from user mode into an invalid window
 * (WIM & (1<<CWP)) != 0
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 * before: |  U  | U | U | L |@T*| U | U | U | U |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 * Do the same as SAVE user window overflow handler (see above), and also
 * mark the last user window as an invalid (so that the current window
 * will be located "between" primarily and secondarily marked ones).
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 *  after: |  U  | U | U | L*|@T | -*| U | U | U |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 ** TRAP from kernel with an available window
 * (WIM & (1<<CWP)) == 0
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 *         |  ?  | ? | ? | K |@T | - | - | -*| ? |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 * Relax! Nothing special to do.
 *
 ** SAVE or TRAP from kernel mode into an invalid window
 * In order to handle window overflow properly we should find out
 * if the window to be saved belongs to the kernel or a user.
 * It is achieved by examining if there is a secondary mark in the WIM
 * besides the primary one (WIM & ~(1<<CWP)).
 * The secondary bit represents the last user window being on the CPU.
 *
 ** SAVE or TRAP from kernel mode into an invalid window
 ** with some user windows.
 * (WIM & (1<<CWP)) != 0 && (WIM & ~(1<<CWP)) != 0
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 * before: |  U  | U | U | L*| K | K |@T*| U | U |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 * Try to save the next window to the user stack,
 * and circular shift the primary bit to the right if saving succeeds.
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 *  after: |  U  | U | U | L*| K | K |@T | -*| U |
 *  (ok)   +-----+---+---+---+---+---+---+---+---+
@endverbatim
 * If user stack corruption is detected, then mark all of the user windows
 * as free just by removing the primary bit. The old secondary mark
 * becomes primary and represents the new invalid window.
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 *  after: |  -  | - | - | -*| K | K |@T | - | - |
 *  (fail) +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 * So what happens when the last user window is about to be saved?
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 * before: |  K  | K |@T*| L*| K | K | K | K | K |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 * Try to save the last user window (L) into the user stack,
 * and for both cases, either saving succeeds or not,
 * remove the primary mark (the trap window (T)).
 * The secondary bit (the old (L) window) becomes primary,
 * and the WIM returns back to its regular single-bit form:
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 *  after: |  K  | K |@T | -*| K | K | K | K | K |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 ** SAVE or TRAP from kernel mode into an invalid window
 ** without any user windows.
 * (WIM & (1<<CWP)) != 0 && (WIM & ~(1<<CWP)) == 0
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 * before: |  K  | K | K |@T*| K | K | K | K | K |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 * Just save the next window to the kernel stack,
 * and circular shift the invalid bit to the right.
@verbatim
 *         +-----+---+---+---+---+---+---+---+---+
 *  after: |  K  | K | K |@T | -*| K | K | K | K |
 *         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 * @sa windows.h
 *
 * @author Eldar Abusalimov
 */

#include <asm/trap_setup.h>
#include <asm/windows.h>
#include <asm/regs.h>
#include <asm/boot_patch.h>

	.text
	.align 4

/** stack align (rounds "x" up to doubleword boundary) */
#define SA(x) (((x)+7)&(~7))
/** trap frame size */
#define TRAPFRAME_SZ (STACKFRAME_SZ + TRAP_REGS_SZ)

#define g_newwim     g1
#define g_temp       g2

/**
 * Prepares trap window to enter c-code.
 *
 * Each generic trap entry point executes the following:
 *	rd %psr, %t_psr
 *	rd %wim, %t_wim
 *	ba trap_handler
 *	 mov 0x1, %t_twinmask
 *
 * Then 'trap_handler' if it needs a trap frame (ie. it has
 * to call c-code and the trap cannot be handled in-window)
 * then it does the SAVE_ALL macro in entry.S which jumps here
 * doing the following:
 *
 *	sethi %hi(trap_setup_begin), %t_retpc
 *	jmpl  %t_retpc + %lo(trap_setup_begin), %t_retpc
 *	 sll  %t_twinmask, %t_psr, %t_twinmask
 *
 * Thus this routine expects that the following regs have already been
 * initialized.
 *	on trap entry time: %t_psr, %t_wim, %t_pc and %t_npc
 *	when jumping here:  %t_twinmask and %t_retpc.
 *
 * It is assumed that these registers are not touched at all
 * before executing SAVE_ALL macro.
 *
 */	.global trap_setup_begin
trap_setup_begin:
	/* In a nutshell: check the mode, build and store TRAP_REGS structure
	 * into the kernel stack, and handle possible window overflow.
	 *
	 * Here goes!
	 */

	/* First of all, check whether we have came from kernel or user code,
	 * and setup the stack pointer accordingly. */
	andcc %t_psr, PSR_PS, %g0
	bne,a tsb_startup_storeregs
	/* For kernel mode:
	 * Allocate a new frame in the existing kernel stack.
	 * It's easy because the previous window in owned by the kernel
	 * and we can trust in %fp value.
	 * (appears in the delay if the branch is taken). */
	 add %fp, -SA(TRAPFRAME_SZ), %t_ksp

	/* For user mode:
	 * Set pointer to the top of the empty kernel stack. */
	set SA(__stack - TRAPFRAME_SZ), %t_ksp

	/* Note that we do not touch the real %sp register,
	 * we'll do it a bit later after checking for possible window overflow.
	 * (we use local t_ksp instead, be careful and do not change it
	 * until updating the real %sp) */
tsb_startup_storeregs:
	/* Write the cpu state (globals, psr, pc, npc, y, ...) */
	STORE_TR_ALL(t_ksp, t_psr, t_pc, t_npc, t_uwinmask)

	/* Branch again to the user/kernel specific setup routines. */
	be tsb_user
	 /* delay slot: for both cases test for window overflow. */
	 andcc %t_wim, %t_twinmask, %g0

tsb_kernel:
	/* Check the result of the window overflow test. */
	bne,a tsb_kernel_wof
	 /* delay slot: determine whether there are some user windows
	  * calculating the mask of the last user window. */
	 andncc %t_wim, %t_twinmask, %t_uwinmask

	/* You're lucky guy, you've come from the kernel,
	 * and there was no overflow when entering the trap window.
	 * Just update the real stack pointer and return back to trap handler.
	 */
	jmpl %t_retpc + 8, %g0
	 mov %t_ksp, %sp

tsb_kernel_wof:
	/* Calculate new WIM. */
	srl %t_twinmask, 1, %g_temp
	sll %t_twinmask, CORE_NWINDOWS - 1, %g_newwim

	/* Check whether the window to be saved is user or kernel one.
	 * See windows.h for details.
	[andncc %t_wim, %t_twinmask, %t_uwinmask] */
	bne tsb_save_user_window
	 or %g_newwim, %g_temp, %g_newwim

tsb_save_kernel_window:
	/* Save kernel window.
	 * No user windows on the CPU, and WIM is in single-bit form.
	 * Just write the shifted value. Nothing complicated.
	 */

	save	// Get into the window to be saved
	wr %g_newwim, %g0, %wim
	 // do not wait for delayed write
	STORE_WINDOW(sp, 0)
	restore	// Go back to trap window

	/* Update the real stack pointer and return back to trap handler. */
	jmpl %t_retpc + 8, %g0
	 mov %t_ksp, %sp

tsb_user:
	/* WIM is in single-bit mode now, need to mark the last user window.
	 * Calculate the mask of the last user window. */
	sll %t_twinmask, 1, %g_temp
	srl %t_twinmask, CORE_NWINDOWS - 1, %t_uwinmask

	/* Check the result of the window overflow test
	 * performed right after branching to user/kernel mode.
	[andcc %t_wim, %t_twinmask, %g0] */
	bne tsb_user_wof
	 or %t_uwinmask, %g_temp, %t_uwinmask

	/* Mark the window we came from as an invalid. */
	wr %t_wim, %t_uwinmask, %wim

	/* Update the real stack pointer and return back to trap handler. */
	jmpl %t_retpc + 8, %g0
	 mov %t_ksp, %sp

tsb_user_wof:
	/* Calculate new WIM. */
	srl %t_twinmask, 1, %g_temp
	sll %t_twinmask, CORE_NWINDOWS - 1, %g_newwim
	or  %g_newwim, %g_temp, %g_newwim

tsb_save_user_window:
 	/* Damn! Need to save user window. :(
	 *
	 * At this point %g_newwim contains shifted single-bit new window mask,
	 * and %t_uwinmask - the mask of the last user window.
	 */
	or %g_newwim, %t_uwinmask, %g_newwim

	save	// Get into the window to be saved
	wr %g_newwim, %g0, %wim
	 // do not wait for delayed write

	/* Attempt to save the registers... */
	STORE_WINDOW(sp, 0)

	restore	// Go back to trap window

	/* Update the real stack pointer and return back to trap handler. */
	jmpl %t_retpc + 8, %g0
	 mov %t_ksp, %sp

tsb_user_stack_is_corrupt:
	/* From user/kernel into invalid window with bad user stack.
	 * Unmark the uwinmask as if there are no user windows on the CPU,
	 * TODO and kill the current process.
	 */
	restore

	wr %g_newwim, %t_uwinmask, %wim
	 WRITE_PAUSE

	jmpl %t_retpc + 8, %g0
	 mov %t_ksp, %sp

#undef g_newwim
#undef g_temp


#define g_newwim     g1
#define t_newwim     t_ksp
#define t_temp       t_retpc

/**
 * Window overflow handling routine.
 *
 * Assumes that the trap entry point has already done the following:
 *
 *	rd %psr, %t_psr
 *	rd %wim, %t_wim
 *	ba window_overflow
 *	 andcc %t_psr, PSR_PS, %g0
 *
 */	.global  window_overflow
window_overflow:

	/* First of all, check the result of kernel/user mode testing
	 * executed by the trap entry, and branch conditionally.
	[andcc %t_psr, PSR_PS, %g0] */
	be wof_from_user
	 /* delay slot: remember the value of %g_newwim to restore it later. */
	 mov %g_newwim, %t_newwim

wof_from_kernel:
	/* There could be some user windows on the CPU.
	 * Hence the WIM is not necessarily in the single-bit form,
	 * and we cannot use %t_wim value to get the new WIM.
	 * Therefore we need to calculate the current window mask by hand.
	 */
	mov 0x1, %t_twinmask
	sll %t_twinmask, %t_psr, %t_twinmask

	/* Calculate new WIM. */
	srl %t_twinmask, 1, %t_temp
	sll %t_twinmask, CORE_NWINDOWS - 1, %g_newwim
	or  %g_newwim, %t_temp, %g_newwim

	/* Check whether the window to be saved is user or kernel one
	 * simultaneously calculating the mask of the last user window. */
	andncc %t_wim, %t_twinmask, %t_uwinmask
	bne,a wof_save_user_window
	 /* leave the WIM in the double-bit form (if the branch is taken). */
	 or %g_newwim, %t_uwinmask, %g_newwim

wof_save_kernel_window:
	/* Save kernel window.
	 * No user windows on the CPU, and WIM is in single-bit form.
	 */

	save	// Get into the window to be saved
	wr %g_newwim, %g0, %wim
	 // do not wait for delayed write
	STORE_WINDOW(sp, 0)
	restore	// Go back to trap window

	/* restore the trap time PSR. */
	wr %t_psr, %g0, %psr
	 // do not wait
	/* Restore saved %g_newwim */
	mov %t_newwim, %g_newwim
	/* Re-execute SAVE */
	jmp %t_pc
	 rett %t_npc

wof_from_user:
	/* When we come from user mode, the WIM is always in single-bit form,
	 * and its value equals to the current window mask.
	 * Calculate the new WIM using %t_wim. */
	srl %t_wim, 1, %t_temp
	sll %t_wim, CORE_NWINDOWS - 1, %g_newwim
	or  %g_newwim, %t_temp, %g_newwim

wof_save_user_window:
 	/* Damn! Need to save user window. :( */

	save	// Get into the window to be saved
	wr %g_newwim, %g0, %wim
	 // do not wait for delayed write

	/* %g_newwim is not needful anymore, we'll use it as temporary register
	 * because using locals is not permitted in the current window.
	 */

	USER_STACK_PRECHECK(wof_user_stack_is_corrupt, g_newwim)

	/* Attempt to save the registers... */
	STORE_WINDOW(sp, 0)

	USER_STACK_POSTCHECK(wof_user_stack_is_corrupt, g_newwim)

	restore	// Go back to trap window

	/* restore the trap time PSR. */
	wr %t_psr, %g0, %psr
	 // do not wait
	/* Restore saved %g_newwim */
	mov %t_newwim, %g_newwim
	/* Re-execute SAVE */
	jmp %t_pc
	 rett %t_npc

wof_user_stack_is_corrupt:
	/* From user/kernel into invalid window with bad user stack.
	 * Remove the primary bit as if there are no user windows on the CPU.
	 * The old secondary bit becomes primary.
	 * TODO kill the current process.
	 */
	restore

	andcc %t_psr, PSR_PS, %g0
	bne,a wof_from_kernel_with_corrupt_user_stack
	 /* Kernel mode wof handler has already prepared %t_uwinmask,
	  * so do not calculate it again, just write it. */
	 wr %t_uwinmask, %g0, %wim
	  // do not wait

	/* Calculate the mask of the last user window. */
	sll %t_twinmask, 1, %t_temp
	srl %t_twinmask, CORE_NWINDOWS - 1, %t_uwinmask
	 or %t_uwinmask, %t_temp, %t_uwinmask

	wr %t_uwinmask, %g0, %wim
	// TODO scheduler and signals

wof_from_kernel_with_corrupt_user_stack:
	/* restore the trap time PSR. */
	wr %t_psr, %g0, %psr
	 // do not wait
	/* Restore saved %g_newwim */
	mov %t_newwim, %g_newwim
	/* Re-execute SAVE */
	jmp %t_pc
	 rett %t_npc


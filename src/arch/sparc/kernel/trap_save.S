/**
 * @file
 * @brief Low-level trap pre-setup and SPARC windows overflow routines.
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
 ** @par @em SAVE from user mode into an invalid window
@verbatim
         +-----+---+---+---+---+---+---+---+---+
 before: |  U  | U | U | L |@T*| U | U | U | U |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       It's guaranteed that all windows belong to the user. Try to save the
 *       next window to the user stack, and circular shift the invalid bit to
 *       the right if all is ok.
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  U  | U | U | L |@T | -*| U | U | U |
  (ok)   +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       If window saving fails, free all user windows by circular shifting the
 *       invalid bit to the left.
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  -  | - | - | -*|@T | - | - | - | - |
  (fail) +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 *
 ** @par @em TRAP from user mode with an available window
 *       <em>(WIM & (1<<CWP)) == 0</em>
@verbatim
         +-----+---+---+---+---+---+---+---+---+
 before: |  U  | U | U | L |@T | - | -*| U | U |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       Mark the window we came from (L) as an invalid.This will be the
 *       secondary mark as opposite to the primary one, which denotes the
 *       actually invalid window. The secondary mark will be used later to
 *       determine whether there are any user windows on the CPU to deal with.
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  U  | U | U | L*|@T | - | -*| U | U |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 *
 ** @par @em TRAP from user mode into an invalid window
 *       <em>(WIM & (1<<CWP)) != 0</em>
@verbatim
         +-----+---+---+---+---+---+---+---+---+
 before: |  U  | U | U | L |@T*| U | U | U | U |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       Do the same as @em SAVE user window overflow handler (see above), and
 *       mark the last user window as an invalid (so that the current window
 *       will be located "between" primarily and secondarily marked ones).
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  U  | U | U | L*|@T | -*| U | U | U |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 *
 ** @par @em TRAP from kernel with an available window
 *       <em>(WIM & (1<<CWP)) == 0</em>
@verbatim
         +-----+---+---+---+---+---+---+---+---+
         |  ?  | ? | ? | K |@T | - | - | -*| ? |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       Relax! Nothing special to do.
 *
 *
 ** @par @em SAVE or @em TRAP from kernel mode into an invalid window
 *       In order to handle window overflow properly we should find out if the
 *       window to be saved belongs to the kernel or a user. It is achieved by
 *       examining if there is a secondary mark in the @em WIM besides the
 *       primary one (<em>WIM & ~(1<<CWP)</em>). The secondary bit represents
 *       the last user window being on the CPU.
 *
 *
 ** @par ... with some user windows
 *       <em>(WIM & (1<<CWP)) != 0 && (WIM & ~(1<<CWP)) != 0</em>
@verbatim
         +-----+---+---+---+---+---+---+---+---+
 before: |  U  | U | U | L*| K | K |@T*| U | U |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       Try to save the next window to the user stack, and circular shift the
 *       primary bit to the right if saving succeeds.
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  U  | U | U | L*| K | K |@T | -*| U |
  (ok)   +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       If user stack corruption is detected, then mark all user windows as
 *       free just by removing the primary bit. The old secondary mark becomes
 *       primary and represents the new invalid window.
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  -  | - | - | -*| K | K |@T | - | - |
  (fail) +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 * @par
 *       So what happens when the last user window is about to be saved?
@verbatim
         +-----+---+---+---+---+---+---+---+---+
 before: |  K  | K |@T*| L*| K | K | K | K | K |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       Try to save the last user window (L) into the user stack, and for both
 *       cases, either saving succeeds or not, remove the primary mark (the
 *       trap window (T)). The secondary bit (the old (L) window) becomes
 *       primary, and the @em WIM returns back to its regular single-bit form.
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  K  | K |@T | -*| K | K | K | K | K |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 *
 ** @par ... without any user windows
 *       <em>(WIM & (1<<CWP)) != 0 && (WIM & ~(1<<CWP)) == 0</em>
@verbatim
         +-----+---+---+---+---+---+---+---+---+
 before: |  K  | K | K |@T*| K | K | K | K | K |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *       Just save the next window to the kernel stack,  and circular shift the
 *       invalid bit to the right.
@verbatim
         +-----+---+---+---+---+---+---+---+---+
  after: |  K  | K | K |@T | -*| K | K | K | K |
         +-----+---+---+---+---+---+---+---+---+
@endverbatim
 *
 *
 * @sa wim.h
 *
 * @author Eldar Abusalimov
 */

#include <util/binalign.h>

#include <asm/regs.h>
#include <asm/psr.h>
#include <asm/wim.h>
#include <asm/asi.h>
#include <asm/ptrace.h>
#include <asm/stack.h>
#include <asm/winmacro.h>

	.text
	.align 4

/* Trap pre-setup. */

/** Stack align (rounds "x" up to doubleword boundary). */
#define STACK_ALIGN(x) binalign_bound(x, STACK_ALIGNMENT)
/** Trap frame size. */
#define TRAP_FRAME_SZ  STACK_ALIGN(STACK_FRAME_SZ + PTRACE_REGS_SZ)

/** Used for new WIM calculation and inter-window reference. */
#define g_newwim     g1
/** The mask of the last user window (if any). */
#define g_uwinmask   g2
/** For temporal computations. */
#define g_temp       g3
/** Kernel stack pointer. */
#define t_ksp        temp

/**
 * Prepares trap window to enter c-code.
 *
 * Each generic trap entry point executes the following:
@code
	ba trap_handler
	 rd %psr, %t_psr
	nop
	nop
@endcode
 *
 * Then @em trap_handler if it needs a trap frame (ie. it has to call c-code
 * and the trap cannot be handled in-window) then it executes the
 * @em SAVE_ALL macro in entry.S which jumps here doing the following:
@code
	sethi %hi(trap_setup_begin), %t_retpc
	jmpl  %t_retpc + %lo(trap_setup_begin), %t_retpc
	 rd %wim, %t_wim
@endcode
 *
 * So thus this routine expects that the following registers have already been
 * initialized:
 * @li on trap entry time: @em \%t_psr, @em \%t_wim, @em \%t_pc and @em \%t_npc
 * @li when jumping here:  @em \%t_retpc.
 *
 * It is assumed that these registers are <b>not touched at all</b> before
 * executing @c SAVE_ALL macro.
 *
 */	.global trap_setup_begin
trap_setup_begin:
	/* In a nutshell: check the mode, build and store TRAP_REGS structure
	 * into the kernel stack, and handle possible window overflow.
	 *
	 * Here goes!
	 */

	/* First of all, check whether we have came from kernel or user code,
	 * and branch to the kernel/user specific setup routines accordingly. */
	andcc %t_psr, PSR_PS, %g0
	/* Calculate the mask of trap time window (1 << CWP). */
	mov 0x1, %t_twinmask
	be tsb_user
	 sll %t_twinmask, %t_psr, %t_twinmask

tsb_kernel:
	/* For kernel mode:
	 * Allocate a new frame in the existing kernel stack.
	 * It's easy because the previous window in owned by the kernel
	 * and we can trust in %fp value.
	 * Note that we do not touch the real %sp register,
	 * we'll do it a bit later after checking for possible window overflow.
	 */

	/* Write the CPU state (globals, PSR, PC, nPC, Y, ...). */
	STORE_PT_ALL(fp - TRAP_FRAME_SZ, t_psr, t_pc, t_npc, g_temp)

	/* Test WIM for window overflow. */
	andcc %t_wim, %t_twinmask, %g0
	bne tsb_kernel_wof
	 add %fp, -TRAP_FRAME_SZ, %t_ksp

	/* You're lucky guy, you've come from the kernel,
	 * and there was no overflow when entering the trap window.
	 * Just update the real stack pointer and return back to trap handler. */
	jmpl %t_retpc + 8, %g0
	 mov %t_ksp, %sp

tsb_kernel_wof:
	/* Determine whether there are some user windows calculating the mask of
	 * the last one (if any). */
	andncc %t_wim, %t_twinmask, %g_uwinmask
	/* Calculate new WIM stubs needed by tsb_save_user_window routine. */
	srl %t_twinmask, 1, %g_newwim
	/* Check whether the window to be saved is user or kernel one. */
	bne tsb_save_user_window
	 sll %t_twinmask, CONFIG_NWINDOWS - 1, %g_temp

tsb_save_kernel_window:
	/* Save kernel window.
	 * No user windows on the CPU, and WIM is in single-bit form.
	 * Just write the shifted value. Nothing complicated.
	 */

	/* Adding %g_temp to %g_newwim gives the same result as ORing them
	 * together. */
	save %g_newwim, %g_temp, %g_newwim ! Get into the window to be saved
	wr %g_newwim, %g0, %wim
	 ! do not wait for delayed write

	STORE_WINDOW(sp)

	restore %g0, %g0, %g0 ! Go back to trap window

	/* Update the real stack pointer and return back to trap handler. */
	jmpl %t_retpc + 8, %g0
	 mov %t_ksp, %sp

tsb_user:
	/* For user mode:
	 * Set pointer to the top of the empty kernel stack. */
	set __stack - TRAP_FRAME_SZ, %t_ksp

	/* Write the cpu state (globals, psr, pc, npc, y, ...) */
	STORE_PT_ALL(t_ksp, t_psr, t_pc, t_npc, g_temp)

	/* WIM is in single-bit mode now, need to mark the last user window.
	 * Calculate the mask of the last user window. */
	sll %t_twinmask, 1, %g_uwinmask
	srl %t_twinmask, CONFIG_NWINDOWS - 1, %g_temp

	/* Test WIM for window overflow. */
	andcc %t_wim, %t_twinmask, %g0
	bne tsb_user_wof
	 or %g_uwinmask, %g_temp, %g_uwinmask

	/* Mark the window we came from as an invalid. */
	wr %t_wim, %g_uwinmask, %wim
	 nop // TODO wait a bit

	/* Update the real stack pointer and return back to trap handler. */
	jmpl %t_retpc + 8, %g0
	 mov %t_ksp, %sp

tsb_user_wof:
	/* Prepare new WIM stubs. These values will XORed together during WRWIM. */
	srl %t_twinmask, 1, %g_newwim
	sll %t_twinmask, CONFIG_NWINDOWS - 1, %g_temp

tsb_save_user_window:
	/* Damn! Need to save user window. :(
	 *
	 * At this point %g_uwinmask contains the mask of the last user window,
	 * and bitwise ORing %g_newwim together with %g_temp results in shifted
	 * single-bit new window mask.
	 */

	/* The two instructions below do the following:
	 * (%g_newwim + %g_uwinmask) ^ %g_temp = %g_newwim | %g_uwinmask | %g_temp
	 * = %g_uwinmask | (%g_newwim | %g_temp)
	 * This is exactly what we need - double-bit WIM denoting the windows
	 * before (last user window) and after (new invalid window) the current
	 * one. */
	save %g_newwim, %g_uwinmask, %g_newwim ! Get into the window to be saved
	wr %g_newwim, %g_temp, %wim
	 ! do not wait for delayed write

	/* Try to save registers to user stack. */
	STORE_USER_WINDOW(sp, tsb_user_stack_is_corrupt, g_temp)

	restore %g0, %g0, %g0 ! Go back to trap window

	/* Update the real stack pointer and return back to trap handler. */
	jmpl %t_retpc + 8, %g0
	 mov %t_ksp, %sp

tsb_user_stack_is_corrupt:
	/* From user/kernel into invalid window with bad user stack.
	 * Unmark the uwinmask as if there are no user windows on the CPU,
	 * TODO and kill the current process.
	 */
	restore

	wr %g_newwim, %g_uwinmask, %wim
	 WRITE_PAUSE

	jmpl %t_retpc + 8, %g0
	 mov %t_ksp, %sp

#undef g_newwim
#undef g_temp
#undef g_ksp

/* Window overflow trap. */

/** Used for new WIM calculation and inter-window reference. */
#define g_newwim     g1
/** Holds initial g_newwim value to be restored before returning from trap. */
#define t_saved      local
/** The mask of the last user window (if any). */
#define t_uwinmask   t_retpc
/** For temporal computations. */
#define t_temp       temp

/**
 * Window overflow handling routine.
 *
 * Assumes that the trap entry point has already done the following:
@code
	rd %psr, %t_psr
	ba window_overflow
	 rd %wim, %t_wim
	nop
@endcode
 *
 */	.global window_overflow
window_overflow:

	/* First of all, check whether we have came from kernel or user code... */
	andcc %t_psr, PSR_PS, %g0
	/* ...and branch respectively. */
	be wof_from_user
	 /* delay slot: remember the value of %g_newwim to restore it later. */
	 mov %g_newwim, %t_saved

wof_from_kernel:
	/* There could be some user windows on the CPU.
	 * Hence the WIM is not necessarily in the single-bit form,
	 * and we cannot use %t_wim value to get the new WIM.
	 * Therefore we need to calculate the current window mask by hand.
	 */
	mov 0x1, %t_twinmask
	sll %t_twinmask, %t_psr, %t_twinmask

	/* Prepare some values needed to calculate new WIM. */
	srl %t_twinmask, 1, %g_newwim
	sll %t_twinmask, CONFIG_NWINDOWS - 1, %t_temp

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

	save %g_newwim, %t_temp, %g_newwim ! Get into the window to be saved
	wr %g_newwim, %g0, %wim
	 ! do not wait

	/* Do our job. */
	STORE_WINDOW(sp)

	restore %g0, %g0, %g0 ! Go back to trap window

	/* restore the trap time PSR. */
	wr %t_psr, %g0, %psr
	 ! do not wait
	/* Restore saved %g_newwim */
	mov %t_saved, %g_newwim

	/* Re-execute SAVE */
	jmp %t_pc
	 rett %t_npc

wof_from_user:
	/* When we come from user mode, the WIM is always in single-bit form,
	 * and its value equals to the current window mask.
	 * Calculate the new WIM using %t_wim. */
	srl %t_wim, 1, %g_newwim
	sll %t_wim, CONFIG_NWINDOWS - 1, %t_temp

wof_save_user_window:
	/* Need to save user window.
	 *
	 * When branching here, bitwise ORing %g_newwim together with %t_temp
	 * should result in new window mask to write (either being in single- or in
	 * double-bit form).
	 */

	save %g_newwim, %t_temp, %g_newwim ! Get into the window to be saved
	wr %g_newwim, %g0, %wim
	 ! do not wait

	/* %g_newwim is not needful anymore, we'll use it as temporary register
	 * because using locals is not permitted in the current window.
	 * Try to save registers to user stack. */
	STORE_USER_WINDOW(sp, wof_user_stack_is_corrupt, g_newwim)

	restore %g0, %g0, %g0 ! Go back to trap window

	/* restore the trap time PSR. */
	wr %t_psr, %g0, %psr
	 ! do not wait
	/* Restore saved %g_newwim */
	mov %t_saved, %g_newwim
	/* Re-execute SAVE */
	jmp %t_pc
	 rett %t_npc

wof_user_stack_is_corrupt:
	/* From user/kernel into invalid window with bad user stack.
	 * Remove the primary bit as if there are no user windows on the CPU.
	 * The old secondary bit becomes primary.
	 * TODO kill the current process.
	 */

	restore %g0, %g0, %g0 ! Go back to trap window

	andcc %t_psr, PSR_PS, %g0
	bne,a wof_from_kernel_with_corrupt_user_stack
	 /* Kernel mode wof handler has already prepared %t_uwinmask,
	  * so do not calculate it again, just write it. */
	 wr %t_uwinmask, %g0, %wim
	  ! do not wait

	/* Calculate the mask of the last user window. */
	sll %t_twinmask, 1, %t_uwinmask
	srl %t_twinmask, CONFIG_NWINDOWS - 1, %t_temp
	or  %t_uwinmask, %t_temp, %t_uwinmask

	wr %t_uwinmask, %g0, %wim
	// TODO signals

wof_from_kernel_with_corrupt_user_stack:
	/* restore the trap time PSR. */
	wr %t_psr, %g0, %psr
	 ! do not wait
	/* Restore saved %g_newwim */
	mov %t_saved, %g_newwim
	/* Re-execute SAVE */
	jmp %t_pc
	 rett %t_npc


#undef g_newwim
#undef t_saved
#undef t_uwinmask
#undef t_temp

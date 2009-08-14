/**
 * @file trap_setup.S
 *
 * @brief Low-level traps pre- and post-setup routines.
 *
 * @author Eldar Abusalimov
 */

#include "asm/traps.h"
#include "asm/sparc_regs.h"
#include "asm/windows.h"

	.section ".text"
	.align 4

/* rounds "x" up to doubleword boundary */
#define SA(x) (((x)+7)&(~0x07))
/* trap frame size */
#define TRAPFRAME_SZ (STACKFRAME_SZ + TRAP_CONTEXT_SZ)

#define t_sp t2

	.global trap_entry_begin
trap_entry_begin:
	/*
	 * First of all, we allocate a new stack frame,
	 * and write the cpu state (globals, psr, pc, npc, y, ...) to it.
	 *
	 * Note that we do not update the real sp register,
	 * we'll do it a bit later after cheking for possible window overflow.
	 * (we use local t_sp instead,
	 * be careful and do not change it until moving to the real sp)
	 */

	add %fp, -SA(TRAPFRAME_SZ), %t_sp
	STORE_PT_ALL(t_sp, t_psr, t_pc, t_npc, t0)

	/*
	 * Traps are still disabled at this line,
	 * and the cpu state has just been stored in the memory.
	 * Therefore we can quietly use globals until reenabling traps
	 * and doing the callback into an arbitrary code.
	 */

	/* test CWP for possible window overflow occurred when entering trap */
	PSR_CWP_WIM_TEST(t_psr, t_wim, t0)

	bz trap_wof_test_ok
	 nop/*
		 * Handle window overflow.
		 * Just do the same as the corresponding trap handler routine.
		 * (See windows.S)
		 */

		// Calculate new WIM
		WIM_SHIFT_RIGHT(t_wim, g1, t0)

		save	// Get into the window to be saved

		wr %g1, %g0, %wim
		 // do not wait for delayed write

		STORE_WINDOW(sp, 0)

		restore	// Go back to trap window

	trap_wof_test_ok:
	/*
	 * OK, return to handler and update the real stack pointer
	 */
	jmpl %t_retpc + 8, %g0
	 mov %t_sp, %sp


	.global trap_entry_end
trap_entry_end:
	/*
	 * We do not need trap-time WIM initial value anymore.
	 * So read _new_ value of WIM to %t_wim
	 * (just for code readability).
	 */
	rd %wim, %t_wim;

	/* Now we need to check if the RETT would land us in an invalid window */
	WIM_SHIFT_RIGHT(t_wim, g2, t0)
	/*
	 * we are in the same window as at trap entering time
	 * so get CWP from an old local copy of PSR
	 */
	PSR_CWP_WIM_TEST(t_psr, g2, t0)

	bz trap_wuf_test_ok
	 nop/*
		 * Handle window underflow.
		 * Just do the same as the corresponding trap handler routine.
		 * (See windows.S)
		 */

		/* Calculate new WIM */
		WIM_SHIFT_LEFT(t_wim, g1, t0)

		wr %g1, %g0, %wim
		 WRITE_PAUSE	// wait for delayed-write before RESTORE

		restore	// get into the window to be restored

		LOAD_WINDOW(sp, 0)

		save	// Get back to the trap window

	trap_wuf_test_ok:
	/*
	 * OK, now we feel sure that RETT will not cause any trap
	 */

	LOAD_PT_ALL(sp, t_psr, t_pc, t_npc, t0)

	/*
	 * Be careful here, do not damage context (globals in particular)
	 * that has been just restored.
	 */

// TODO what's about checking and restoring PS/S? -- Eldar
	/* restore PIL&ICC fields of old PSR */
	PSR_BIT_COPY(t_psr, (PSR_PIL | PSR_ICC), t0, t1)
	 WRITE_PAUSE

	/* tata! */
	jmp %t_pc
	 rett %t_npc

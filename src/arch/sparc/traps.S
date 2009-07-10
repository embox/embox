/**
 * \file traps.S
 *
 * \brief Low-level traps dispatching
 *
 * \author Eldar Abusalimov
 * \author Anton Bondarev
 * \author Alexey Fomin
 * \author Alexandr Batyukov
 */

#include "traps.h"
#include "sparc_regs.h"
#include "windows.h"
#include "leon_config.h"
#include "cpu_context.h"

	.section ".text"
	.align 4

#include "traps.inc"

! skip instruction
skip:
	flush
	jmp    %l2
	 rett    %l2 + 4
	nop
	nop

irq_empty:
	jmp		%l1	! old PC
	 rett	%l2	! old nPC
	nop
	nop

//TODO halt_trap need to rewrite
		.global halt_trap
halt_trap:
  !      flush

 !       set   boot_start, %g1
 !       set   boot_start+0x4, %g2
        jmp   %g1
        nop


/* rounds "x" up to doubleword boundary */
#define SA(x) (((x)+7)&(~0x07))
/* trap frame size */
#define TRAPFRAME_SZ (STACKFRAME_SZ + TRAP_CONTEXT_SZ)

#define t_sp t2

/*
 * need move pc and npc to the next instruction
 * because while traps was occure %l1 -> pc mean before instruction execute
 */
 	.global trap_dispatcher
trap_dispatcher:
	! store previous nPC as PC
	mov		%t_npc, %t_pc
	! store previous nPC+4 as nPC
	add     %t_npc, 4, %t_npc

irq_dispatcher:
interrupting_trap:

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
	 * OK, let's update the real stack pointer
	 */
	mov %t_sp, %sp

	// pass trap type as parameter
	mov %tbr, %o0
	and %o0, TBR_TT, %o0
	srl %o0, 4, %o0

		/*
		 * enable traps
		 */
		PSR_BIT_TOGGLE_SHORT(PSR_ET, t0)
		 // do not wait (PSR_ET)

			call	dispatch_trap
			 nop

		/*
		 * disable traps
		 *
		 * and do not worry that the “RDPSR, WRPSR” sequence is interruptible,
		 * our interrupts dispatcher does not corrupt PSR
		 * and restores it to the initial value before returning from trap
		 * (see below)
		 */
		PSR_BIT_TOGGLE_SHORT(PSR_ET, t0)
		 // do not wait (PSR_ET)

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

		wr %g1, %g1, %wim
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
	PSR_BIT_COPY(t_psr, (PSR_PIL | PSR_ICC), t0, t1, t2)
	 WRITE_PAUSE

	/* tata! */
	jmp %t_pc
	 rett %t_npc

#define i_pcontext i0

	.global cpu_context_save_trap
cpu_context_save_trap:

	/*
	 * PSR, PC and nPC have been written
	 * to t_psr, t_pc and t_npc registers respectively at the trap time.
	 */

	set cpu_context_save, %t0
	jmpl %t0, %l_retpc
	 mov %i_pcontext, %l_base

    /* restore PIL&ICC fields of old PSR */
	PSR_BIT_COPY(t_psr, (PSR_PIL | PSR_ICC), t0, t1, t2)
	 WRITE_PAUSE

	/* skip instruction */
	mov %t_npc, %t_pc
	add  %t_npc, 4, %t_npc

	jmp %t_pc
	 rett %t_npc

	.global cpu_context_restore_trap
cpu_context_restore_trap:

	/*
	 * cpu_context_save time PSR, PC and nPC will be restored
	 * to t_psr, t_pc and t_npc registers respectively.
	 */

	set cpu_context_restore, %t0
	jmpl %t0, %l_retpc
	 mov %i_pcontext, %l_base

    /* restore PIL&ICC fields of old PSR */
	PSR_BIT_COPY(t_psr, (PSR_PIL | PSR_ICC), t0, t1, t2)
	 WRITE_PAUSE

	/* skip instruction */
	mov %t_npc, %t_pc
	add  %t_npc, 4, %t_npc

	jmp %t_pc
	 rett %t_npc

bad_trap_dispatcher:
	mov %tbr, %o0
	srl %o0, 4, %o0
	and %o0, 0xFF, %o0		! pass TT as parameter
	mov %t_pc, %o1
	mov %t_npc, %o2
	mov %t_psr, %o3

	PSR_BIT_SET_SHORT(PSR_PIL, t0)
	 WRITE_PAUSE
	PSR_BIT_SET_SHORT(PSR_ET, t0)

	call dispatch_bad_trap
	 nop

.align  32

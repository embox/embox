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

		mov %g1, %wim
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

		// enable traps
		set PSR_ET, %g1
		PSR_BIT_SET(g1, t0)

			call	dispatch_trap
			 nop

		// disable traps
		set PSR_ET, %g1
		PSR_BIT_CLEAR(g1, t0)

	/*
	 * We do not need trap-time WIM initial value anymore.
	 * So read _new_ value of WIM to %t_wim
	 * (just for code readability).
	 */
	mov %wim, %t_wim;

	/* test CWP for possible window underflow when executing RETT */
	WIM_SHIFT_RIGHT(t_wim, g2, t0)
	mov %psr, %g1
	PSR_CWP_WIM_TEST(g1, g2, t0)	// PSR is in %g1

	bz trap_wuf_test_ok
	 nop/*
		 * Handle window underflow.
		 * Just do the same as the corresponding trap handler routine.
		 * (See windows.S)
		 */

		/* Calculate new WIM */
		WIM_SHIFT_LEFT(t_wim, g1, t0)

		mov %g1, %wim
		 nop; nop; nop	// wait for delayed-write before RESTORE

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

    /* restore PIL&ICC fields of old PSR */
	set (PSR_PIL | PSR_ICC), %t0
	PSR_BIT_COPY(t_psr, t0, t1, t2)

	/* tata! */
	jmp %t_pc
	 rett %t_npc

bad_trap_dispatcher:
	mov		%tbr, %l3
	srl		%l3, 4, %l5
	and 	%l5, 0xFF, %o0		! pass TT as parameter
	mov		%l1, %o1
	mov		%l2, %o2
	mov		%psr, %o3
	mov		%psr, %l5
	or		%l5, 0x20, %l5	! set PSR.ET (enable traps)
	or		%l5, 0xF00, %l5
	mov		%l5, %psr
	call	dispatch_bad_trap
	 nop

.align  32

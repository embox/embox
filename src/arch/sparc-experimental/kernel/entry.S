/**
 * @file entry.S
 *
 * @brief Low-level traps dispatching routines
 *
 * @author Eldar Abusalimov
 */

#include <asm/traps.h>
#include <asm/regs.h>
#include <asm/winmacro.h>
#include <asm/cpu_context.h>

	.text
	.align 4

	.global instruction_trap
instruction_trap:
	! store previous nPC as PC
	mov %t_npc, %t_pc
	! store previous nPC+4 as nPC
	add %t_npc, 4, %t_npc

/**
 * Generic inerrupt handler.
 */	.global interrupting_trap
interrupting_trap:
	SAVE_ALL

	/*
	 * Traps are still disabled at this line,
	 * and the cpu state has just been stored in the memory.
	 * Therefore we can quietly use globals until enabling traps
	 * and doing the callback into an arbitrary code.
	 *
	 * Feel free! ;)
	 */

	// pass trap type as parameter
	TBR_GET_TT(o0);

	/*
	 * enable traps
	 */
	PSR_BIT_TOGGLE_SHORT(PSR_ET, g1)
	 // do not wait (PSR_ET)

		call	dispatch_trap
		 nop

	/*
	 * disable traps
	 *
	 * and do not worry that the RDPSR, WRPSR sequence is interruptible,
	 * our interrupts dispatcher does not corrupt PSR
	 * and restores it to the initial value before returning from trap
	 * (see traps.S, trap_entry_fini)
	 */
	PSR_BIT_TOGGLE_SHORT(PSR_ET, g1)
	 // do not wait (PSR_ET)

	RESTORE_ALL

#define i_pcontext i0

	.global cpu_context_save_trap
cpu_context_save_trap:

	/*
	 * PSR, PC and nPC have been written
	 * to t_psr, t_pc and t_npc registers respectively at the trap time.
	 */

	set cpu_context_save, %l_retpc
	jmpl %l_retpc, %l_retpc
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

	set cpu_context_restore, %l_retpc
	jmpl %l_retpc, %l_retpc
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

	PSR_BIT_SET_SHORT(PSR_PIL, g1)
	 WRITE_PAUSE
	PSR_BIT_SET_SHORT(PSR_ET, g1)

	call dispatch_bad_trap
	 nop


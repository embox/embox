/**
 * @file entry.S
 *
 * @brief Low-level traps dispatching
 *
 * @author Eldar Abusalimov
 * @author Anton Bondarev
 * @author Alexey Fomin
 * @author Alexandr Batyukov
 */

#include "traps.h"
#include "sparc_regs.h"
#include "windows.h"
#include "leon_config.h"
#include "cpu_context.h"

	.section ".text"
	.align 16

#include "traps.inc"

trap_dispatcher:
	! store previous nPC as PC
	mov		%t_npc, %t_pc
	! store previous nPC+4 as nPC
	add     %t_npc, 4, %t_npc

irq_dispatcher:
interrupting_trap:

	SAVE_ALL

	mov %sp, %o1

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
	 * and do not worry that the a�?RDPSR, WRPSRa�? sequence is interruptible,
	 * our interrupts dispatcher does not corrupt PSR
	 * and restores it to the initial value before returning from trap
	 * (see below)
	 */
	PSR_BIT_TOGGLE_SHORT(PSR_ET, t0)
	 // do not wait (PSR_ET)

	RESTORE_ALL

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
	PSR_BIT_COPY(t_psr, (PSR_PIL | PSR_ICC), t0, t1)
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
	PSR_BIT_COPY(t_psr, (PSR_PIL | PSR_ICC), t0, t1)
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


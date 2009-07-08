/**
 * \file sys.S
 *
 * \date Jun 09, 2009
 * \author Eldar Abusalimov
 * \author Alexey Fomin
 */

#include "cpu_context.h"
#include "sparc_regs.h"
#include "traps.h"

	.section ".text"
	.align 4

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
	set (PSR_PIL | PSR_ICC), %t0
	PSR_BIT_COPY(t_psr, t0, t1, t2)

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
	set (PSR_PIL | PSR_ICC), %t0
	PSR_BIT_COPY(t_psr, t0, t1, t2)

	/* skip instruction */
	mov %t_npc, %t_pc
	add  %t_npc, 4, %t_npc

	jmp %t_pc
	 rett %t_npc


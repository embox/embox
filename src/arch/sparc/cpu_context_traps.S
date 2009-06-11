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
	 * PSR, PC and nPC are implicitly passed
	 * through l_psr, l_pc and l_npc registers respectively.
	 */

	set cpu_context_save, %t0
	jmpl %t0, %l_retpc
	 mov %i_pcontext, %l_base

	mov %t_npc, %t_pc
	add  %t_npc, 4, %t_npc
	jmp %t_pc
	 rett %t_npc

	.global cpu_context_restore_trap
cpu_context_restore_trap:

	set cpu_context_restore, %t0
	jmpl %t0, %l_retpc
	 mov %i_pcontext, %l_base

	/*
	 * _save_ time PSR, PC and nPC has been returned
	 * through l_psr, l_pc and l_npc registers respectively.
	 * restore PIL & ICC fields of PSR.
	 */
	mov %psr, %t0
	set PSR_PIL || PSR_ICC, %t1
	andn %t0, %t1, %t0
	and %t_psr, %t1, %t1
	or  %t0, %t1, %t0
	mov %t0, %psr
	 nop; nop; nop	// wait for PSR delayed-write

	mov %t_npc, %t_pc
	add  %t_npc, 4, %t_npc
	jmp %t_pc
	 rett %t_npc


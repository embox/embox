/**
 * \file sys.S
 *
 * \date Jun 09, 2009
 * \author Eldar Abusalimov
 * \author Alexey Fomin
 */

#include "cpu_context.h"
#include "traps.h"

	.section ".text"
	.align 4

#define i_pcontext i0

	.global cpu_context_save_trap
cpu_context_save_trap:

	set cpu_context_save, %t_0
	jmpl %t_0, %l_retpc
	 mov %i_pcontext, %l_base

	mov %t_npc, %t_pc
	add  %t_npc, 4, %t_npc
	jmp %t_pc
	 rett %t_npc

	.global cpu_context_restore_trap
cpu_context_restore_trap:

	set cpu_context_restore, %t_0
	jmpl %t_0, %l_retpc
	 mov %i_pcontext, %l_base

	mov %t_npc, %t_pc
	add  %t_npc, 4, %t_npc
	jmp %t_pc
	 rett %t_npc


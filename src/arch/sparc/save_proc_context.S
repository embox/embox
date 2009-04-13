/**
 * \file save_proc_context.S
 *
 * \date Jan 21, 2009
 * \author anton, fomka
 */

#include "cpu_context.h"

	.section    ".text"
	.align  4

	/**
	* function save processor context to pointed memory
	* @param %o0 - pointer to struct of processor context
	*/
	.global save_proc_context
save_proc_context:
//	set cpu_context, %l0
//%fp (frame pointer; conventionally same as %i6)
//%sp (stack pointer; conventionally same as %o6)
	st %sp,  [%o0 + CPU_CONTEXT_SP]
	st %fp, [%o0 + CPU_CONTEXT_FP]
	// st %g0, [%o0 + 0x18]
	st %g1, [%o0 + CPU_CONTEXT_GLOB + 0x04]
	st %g2, [%o0 + CPU_CONTEXT_GLOB + 0x08]
	st %g3, [%o0 + CPU_CONTEXT_GLOB + 0x0C]
	st %g4, [%o0 + CPU_CONTEXT_GLOB + 0x10]
	st %g5, [%o0 + CPU_CONTEXT_GLOB + 0x14]
	st %g6, [%o0 + CPU_CONTEXT_GLOB + 0x18]
	st %g7, [%o0 + CPU_CONTEXT_GLOB + 0x1C]
	//pc + npc after trap
	st %o1, [%o0 + CPU_CONTEXT_PC]
	st %o2, [%o0 + CPU_CONTEXT_NPC]

	//pc + npc after trap
	//st %l1, [%o0 + 0x38]
	//st %l2, [%o0 + 0x3C]


	//save psr wim y tbr
	mov %wim, %g1
	st %g1, [%o0 + CPU_CONTEXT_WIM]
	mov %psr, %g1
	st %g1, [%o0 + CPU_CONTEXT_PSR]
	mov %y, %g1
	st %g1, [%o0 + CPU_CONTEXT_Y]
	mov %tbr, %g1
	st %g1, [%o0 + CPU_CONTEXT_TBR]


//save all window
	mov %g0, %wim
	set WINDOWS_QUANTITY, %g1//NWINDOW
	add %o0, CPU_CONTEXT_LOCAL, %g2


register_window_save:
	st %l0, [%g2 + 0x0]
	st %l1, [%g2 + 0x4]
	st %l2, [%g2 + 0x8]
	st %l3, [%g2 + 0xc]
	st %l4, [%g2 + 0x10]
	st %l5, [%g2 + 0x14]
	st %l6, [%g2 + 0x18]
	st %l7, [%g2 + 0x1c]
	st %o0, [%g2 + 0x20]
	st %o1, [%g2 + 0x24]
	st %o2, [%g2 + 0x28]
	st %o3, [%g2 + 0x2c]
	st %o4, [%g2 + 0x30]
	st %o5, [%g2 + 0x34]
	st %o6, [%g2 + 0x38]
	st %o7, [%g2 + 0x3c]
	add %g2, LOCAL_REGS_QUANTITY * 4, %g2
	deccc %g1
 	bg register_window_save
	save       !
	nop

// restore wim
	ld [%o0 + CPU_CONTEXT_WIM],	%g1
	mov %g1,	%wim

// restore g1, g2
//	set cpu_context, %o0
	ld [%o0 + CPU_CONTEXT_GLOB + 0x04],%g1
	ld [%o0 + CPU_CONTEXT_GLOB + 0x08],%g2

	jmpl %r15 + 8,%g0 //function without instruction "save"
	nop


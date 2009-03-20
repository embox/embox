/*
 * save_proc_context.S
 *
 *  Created on: Jan 21, 2009
 *      Author: anton, fomka
 */

//#include "cpu_context.h"
	.seg    "text"
	.proc   0
	.align  4

	.global save_proc_context
save_proc_context:
	set cpu_context, %l0
//%fp (frame pointer; conventionally same as %i6)
//%sp (stack pointer; conventionally same as %o6)
	st %sp,  [%l0 + 0x10]
	st %fp, [%l0 + 0x14]
	// st %g0, [%l0 + 0x18]
	st %g1, [%l0 + 0x1C]
	st %g2, [%l0 + 0x20]
	st %g3, [%l0 + 0x24]
	st %g4, [%l0 + 0x28]
	st %g5, [%l0 + 0x2c]
	st %g6, [%l0 + 0x30]
	st %g7, [%l0 + 0x34]
	//pc + npc after trap
	st %l1, [%l0 + 0x38]
	st %l2, [%l0 + 0x3C]


	//save psr wim y tbr
	mov %wim, %g1
	st %g1, [%l0 + 0]
	mov %psr, %g1
	st %g1, [%l0 + 4]
	mov %y, %g1
	st %g1, [%l0 + 8]
	mov %tbr, %g1
	st %g1, [%l0 + 0xc]


//save all window
	mov %g0, %wim
	set 8, %g1//NWINDOW
	add %l0, 0x40, %g2


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
	add %g2, 0x40, %g2
	deccc %g1
 	bg register_window_save
	save       !
	nop

// restore wim
	ld [%l0 + 0x0],	%g1
	mov %g1,	%wim

// restore g1, g2
	set cpu_context, %l0
	ld [%l0 + 0x20],%g1
	ld [%l0 + 0x22],%g2

	jmpl %r15 + 8,%g0 //if call without save
	nop


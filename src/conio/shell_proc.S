/*
 * shell_proc.S
 *
 *  Created on: 26.02.2009
 *      Author: Alexey Fomin
 */

	.global shell_save_proc
	.global shell_restore_proc
shell_save_proc:
	set shell_proc_state, %l0
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

//	//pc + npc
//	st %l1, [%l0 + 0x38]
//	st %l2, [%l0 + 0x3C]

	// psr + wim + y + tbr
	mov %wim, %g1
	st %g1, [%l0 + 0]
	mov %psr, %g1
	st %g1, [%l0 + 4]
	mov %y, %g1
	st %g1, [%l0 + 8]
	mov %tbr, %g1
	st %g1, [%l0 + 0xc]


//save all window

// set wim to 0 (owerflow now won't happen)
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
	set shell_proc_state, %l0
	ld [%l0 + 0x20],%g1
	ld [%l0 + 0x22],%g2


	call shell_handler_start
	 nop

//	jmpl %r15 + 8,%g0 //if call without save
//	nop

shell_restore_proc:
	set shell_proc_state, %g2

	ld [%g2 + 0x10],%sp
	ld [%g2 + 0x14],%fp
	// ld [%g2 + 0x18],%g0
	ld [%g2 + 0x24],%g3
	ld [%g2 + 0x28],%g4
	ld [%g2 + 0x2c],%g5
	ld [%g2 + 0x30],%g6
	ld [%g2 + 0x34],%g7

	ld [%g2 + 4],%g1
	mov %g1,%psr
	nop;nop;nop

//restore all window
	mov %g0, %wim
	set 8, %g1//NWINDOW
	add %g2, 0x40, %g2

register_window_restore:
	ld [%g2+0x0],%l0
	ld [%g2+0x4],%l1
	ld [%g2+0x8],%l2
	ld [%g2+0xc],%l3
	ld [%g2+0x10],%l4
	ld [%g2+0x14],%l5
	ld [%g2+0x18],%l6
	ld [%g2+0x1c],%l7
	ld [%g2+0x20],%o0
	ld [%g2+0x24],%o1
	ld [%g2+0x28],%o2
	ld [%g2+0x2c],%o3
	ld [%g2+0x30],%o4
	ld [%g2+0x34],%o5
	ld [%g2+0x38],%o6
	ld [%g2+0x3c],%o7
	add %g2, 0x40, %g2
	deccc %g1
 	bg register_window_restore
	save       !
	nop

	set shell_proc_state, %l0
	//restore psr wim y tbr
	ld [%l0 + 0], %g1
	mov %g1, %wim
	ld [%l0 + 4], %g1
	mov %g1,%psr
	ld [%l0 + 8],%g1
	mov %g1, %y
	ld [%l0 + 0xc],%g1
	mov %g1,	%tbr
	nop;nop;nop

    ld [%l0 + 0x18],%g1
    ld [%l0 + 0x1C],%g2

    call shell_handler_exit
     nop

//	set shell_handler_continue, %l0
//	ld [%l0],	%l1

//	ld [%l0 + 0x38],%l1
//	ld [%l0 + 0x3C],%l2
//	call shell_handler_continue
//	 nop
//	jmpl %l1 + 8,%g0
//	nop


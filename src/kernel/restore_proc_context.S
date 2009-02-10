/*
 * restore_proc_context.S
 *
 *  Created on: Jan 27, 2009
 *      Author: anton
 */
	.seg    "text"
	.proc   0
	.align  4
 	.global restore_proc_context
restore_proc_context:
	//for testing
	set register15, %l0
	st %r15,[%l0 + 0]

	set cpu_context, %l0

	ld [%l0 + 0x10],%sp
	ld [%l0 + 0x10],%fp
	ld [%l0 + 0x18],%g0
	ld [%l0 + 0x24],%g3
	ld [%l0 + 0x28],%g4
	ld [%l0 + 0x2c],%g5
	ld [%l0 + 0x30],%g6
	ld [%l0 + 0x34],%g7

	ld [%l0 + 4],%g1
	mov %g1,%psr
	nop;nop;nop

//restore all window
	mov %g0, %wim
	set 8, %g1//NWINDOW
	add %l0, 0x40, %g2

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

!!!!!for test
!!!!!to return from this procedure
!in other way we would return after save_proc...
	set register15, %l0
	ld [%l0 + 0], %r15

	set cpu_context, %l0
	//restore psr wim y tbr
	ld [%l0 + 0], %g1
	mov %g1, %wim
	ld [%l0 + 4], %g1
!	mov %g1,%psr
!	ld [%l0 + 8],%g1
	mov %g1, %y
	ld [%l0 + 0xc],%g1
	mov %tbr, %g1
	nop;nop;nop


    ld [%l0 + 0x18],%g1
    ld [%l0 + 0x1C],%g2
//address return from trap
	ld [%l0 + 0x38],%l1
	ld [%l0 + 0x3C],%l2

	jmpl %r15 + 8,%g0 // without save
	nop

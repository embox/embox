/**
 * \file restore_proc_context.S
 *
 * \date Jan 27, 2009
 * \autor anton, fomka
 */
#include "cpu_context.h"

//static int ret_addr;
//static int cpu_context_addr;
 	.section ".data"
 	.align  4
// 	.local ret_addr //static
// 	.common ret_addr, 4, 4 //name, size, alignment
 	.local cpu_context_addr
 	.common cpu_context_addr, 4, 4


	.section    ".text"
	.align  4

	/**
	* function restore processor context from pointed memory
	* @param %o0 - pointer to struct of processor context
	*/
 	.global restore_proc_context
restore_proc_context:
	//for testing
	// Saving new r15 to return
	// to position, where restore_proc_context was called, not save_proc_context
//	set ret_addr, %l0
//	st %r15,[%l0]

	//set cpu_context_addr, %l0
	//st %o0,[%l0]
	mov %o0, %g3

//	ld [%o0 + CPU_CONTEXT_SP],%sp
//	ld [%o0 + CPU_CONTEXT_FP],%fp
	// ld [%o0 + 0x18],%g0
	//ld [%o0 + CPU_CONTEXT_GLOB + 0x0C],%g3
	ld [%o0 + CPU_CONTEXT_GLOB + 0x10],%g4
	ld [%o0 + CPU_CONTEXT_GLOB + 0x14],%g5
	ld [%o0 + CPU_CONTEXT_GLOB + 0x18],%g6
	ld [%o0 + CPU_CONTEXT_GLOB + 0x1C],%g7

//before psr change
	add %o0, CPU_CONTEXT_LOCAL, %g2

	ld [%o0 + CPU_CONTEXT_PSR],%g1
	mov %g1,%psr
	nop;nop;nop

//restore all window
	mov %g0, %wim
	set WINDOWS_QUANTITY, %g1//NWINDOW


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
	add %g2, LOCAL_REGS_QUANTITY * 4, %g2
	deccc %g1
 	bg register_window_restore
	save       !
	nop


//	set ret_addr, %o0
//	ld [%o0], %r15

//	set cpu_context_addr, %g2
//	ld [%g2],%o0

	mov %g3,%o0
	//restore psr wim y tbr
	ld [%o0 + CPU_CONTEXT_WIM], %g1
	mov %g1, %wim
	ld [%o0 + CPU_CONTEXT_PSR], %g1
	mov %g1,%psr
	nop;nop;nop
	ld [%o0 + CPU_CONTEXT_Y],%g1
	mov %g1, %y
	ld [%o0 + CPU_CONTEXT_TBR],%g1
	mov %g1, %tbr
	nop;nop;nop

    ld [%o0 + CPU_CONTEXT_GLOB + 4],%g1
    ld [%o0 + CPU_CONTEXT_GLOB + 8],%g2
    ld [%o0 + CPU_CONTEXT_GLOB + 0x0C],%g3
//TODO may be not need
//address return from trap
//	ld [%o0 + 0x38],%l1
//	ld [%o0 + 0x3C],%l2

//	ld [%o0 + CPU_CONTEXT_PC],%o1
//	ld [%o0 + CPU_CONTEXT_NPC],%o2
//	jmpl %r15 + 8,%g0 // function is without instruction "save"
//	nop
	jmp %o1
	nop

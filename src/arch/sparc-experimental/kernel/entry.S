/**
 * @file
 *
 * @brief Low-level traps dispatching routines
 *
 * @author Eldar Abusalimov
 */

#include <asm/entry.h>
#include <asm/regs.h>
#include <asm/psr.h>
#include <asm/winmacro.h>
#include <asm/stack.h>

	.text
	.align 4

/**
 * Generic inerrupt handler.
 */
 	.global interrupt_entry
interrupt_entry:
	SAVE_ALL

	andn %t_psr, PSR_PIL, %g1
	sll %local, 8, %temp
	wr %g1, %temp, %psr
	 or %g1, %temp, %g1; nop; nop
	wr %g1, PSR_ET, %psr
	 ! do not wait

	mov %local, %o0 ! interrupt nr
	call interrupt_handler
	 add %sp, STACK_FRAME_SZ, %o1

	! TODO maybe traps disable is needed

	RESTORE_ALL

	.global weak_trap_entry
weak_trap_entry:
	orcc %local, %g0, %g0
	be bad_trap_entry
	 nop
	call %g1
	 nop

 	.global srmmu_fault
srmmu_fault:
 	.global bad_trap_entry
bad_trap_entry:
	SAVE_ALL

	mov %tbr, %o0
	srl %o0, 4, %o0
	and %o0, 0xFF, %o0		! pass TT as parameter

	or %t_psr, PSR_PIL, %g1
	wr %g1, %g0, %psr
	 WRITE_PAUSE
	wr %g1, PSR_ET, %psr
	 WRITE_PAUSE

!	call bad_trap_handler
	 add %sp, STACK_FRAME_SZ, %o1

	RESTORE_ALL

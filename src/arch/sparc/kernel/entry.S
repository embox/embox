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

	andn %t_psr, PSR_PIL, %temp
	sll %local, 8, %g1
	or %temp, %g1, %temp
	wr %temp, %g0, %psr
	 WRITE_PAUSE
	wr %temp, PSR_ET, %psr
	 ! do not wait

	/* Forward interrupt handling directly to kernel.
	 * There is no need to introduce HAL interrupt handling proxy function.
	 * At least for now. */
	mov %local, %o0 ! interrupt nr
	call irq_dispatch
	 nop
	! add %sp, STACK_FRAME_SZ, %o1

	! TODO maybe traps disable is needed
	! XXX it is really needed because of possible races with another interrupt
	! which may occur after RESTORE_ALL and before restoring trap time PSR.
	! t_wim becomes irrelevant if deep interrupt occurs after reading WIM
	! XXX clean up and describe these bugs -- Eldar.
	wr %temp, %g0, %psr
	 WRITE_PAUSE

	RESTORE_ALL

	.global weak_trap_entry
weak_trap_entry:
	orcc %local, %g0, %g0
	be bad_trap_entry
	 nop
	jmpl %local, %g0
	 nop

! 	.global srmmu_fault
!srmmu_fault:
!	mov %t_npc, %t_pc
!	add %t_npc, 4, %t_npc
	jmpl %t_npc, %g0
	 rett %t_npc + 4
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

	call bad_trap_handler
	 add %sp, STACK_FRAME_SZ, %o1

	RESTORE_ALL

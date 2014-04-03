/**
 * @file
 * @brief Low-level entry for general interrupt handling.
 *
 * @date 29.07.09
 * @author Eldar Abusalimov
 */

#include <asm/entry.h>
#include <asm/regs.h>
#include <asm/psr.h>
#include <asm/winmacro.h>
#include <asm/stack.h>

#include <kernel/critical.h>

	.text
	.align 4

#define t_cptr   t_wim  /**< Pointer to critical counter. */
#define t_cc     t_pc   /**< Critical counter. */
#define t_ncc    t_npc  /**< Incremented critical counter. */

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
	 ! no wait

	set __critical_count, %t_cptr
	ld [%t_cptr], %t_cc
	add %t_cc, __CRITICAL_COUNT(CRITICAL_IRQ_HANDLER), %t_ncc
	st %t_ncc, [%t_cptr]

	wr %temp, PSR_ET, %psr
	 nop;

	/* Forward interrupt handling directly to kernel.
	 * There is no need to introduce HAL interrupt handling proxy function.
	 * At least for now. */
	call irq_dispatch
	 mov %local, %o0 ! interrupt nr

	/* Restore trap enter-time PIL, traps are on. */
	wr %t_psr, PSR_ET, %psr
	 nop;

	call critical_dispatch_pending
	 st %t_cc, [%t_cptr]

	! TODO maybe traps disable is needed
	! XXX it is really needed because of possible races with another interrupt
	! which may occur after RESTORE_ALL and before restoring trap time PSR.
	! t_wim becomes irrelevant if deep interrupt occurs after reading WIM
	! XXX clean up and describe these bugs -- Eldar.

	/* Disable traps. */
	wr %temp, %g0, %psr
	 nop; nop; nop;

	RESTORE_ALL

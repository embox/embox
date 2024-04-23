/**
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */

#include <asm/asm.h>

#include "context.h"

/* a0/a1 points to old/new thread’s control block */
/* Allocate stack space for saving 11 registers. 11*4 = 44 */
ENTRY(context_switch)
	addi    $sp, $sp, -CTX_SIZE
	/* Save the registers */
	sw      $ra, CTX_RA($a0)
	sw      $gp, CTX_GP($a0)
	sw      $s8, CTX_S8($a0)
	sw      $s7, CTX_S7($a0)
	sw      $s6, CTX_S6($a0)
	sw      $s5, CTX_S5($a0)
	sw      $s4, CTX_S4($a0)
	sw      $s3, CTX_S3($a0)
	sw      $s2, CTX_S2($a0)
	sw      $s1, CTX_S1($a0)
	sw      $s0, CTX_S0($a0)
	mfc0    $s0, $12
	sw      $s0, CTX_CR0_STAT($a0)
	/* Store the old stack pointer in the old control block */
	sw      $sp, CTX_SP($a0)
	/* Get the new stack pointer from the new control block */
	lw      $sp, CTX_SP($a1)
	nop     /* delay slot for load */
	/* Now, restore the registers */
	andi    $s0, $s0, 0xff00 /* preserve interrupt mask */
	lw      $s1, CTX_CR0_STAT($a1)
	andi    $s1, $s1, 0x00ff /* load only status bits */
	or      $s0, $s0, $s1 /* combine them together */
	mtc0    $s0, $12
	lw      $s0, CTX_S0($a1)
	lw      $s1, CTX_S1($a1)
	lw      $s2, CTX_S2($a1)
	lw      $s3, CTX_S3($a1)
	lw      $s4, CTX_S4($a1)
	lw      $s5, CTX_S5($a1)
	lw      $s6, CTX_S6($a1)
	lw      $s7, CTX_S7($a1)
	lw      $s8, CTX_S8($a1)
	lw      $gp, CTX_GP($a1)
	lw      $ra, CTX_RA($a1)
	nop     /* delay slot for load */
	addi    $sp, $sp, CTX_SIZE
	/* and return. */
	jr      $ra
	nop     /* in delay slot */
END(context_switch)

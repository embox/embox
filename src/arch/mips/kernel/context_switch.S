/**
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */

#include <asm/asm.h>

#include "context.h"

ENTRY(context_switch)
	LONG_ADDI $sp, $sp, -CTX_SIZE
	/* Save the registers */
	LONG_S  $ra, CTX_RA($a0)
	LONG_S  $gp, CTX_GP($a0)
	LONG_S  $s8, CTX_S8($a0)
	LONG_S  $s7, CTX_S7($a0)
	LONG_S  $s6, CTX_S6($a0)
	LONG_S  $s5, CTX_S5($a0)
	LONG_S  $s4, CTX_S4($a0)
	LONG_S  $s3, CTX_S3($a0)
	LONG_S  $s2, CTX_S2($a0)
	LONG_S  $s1, CTX_S1($a0)
	LONG_S  $s0, CTX_S0($a0)
	mfc0    $s0, $12
	LONG_S  $s0, CTX_CR0_STAT($a0)
	/* Store the old stack pointer in the old control block */
	LONG_S  $sp, CTX_SP($a0)
	/* Get the new stack pointer from the new control block */
	LONG_L  $sp, CTX_SP($a1)
	nop     /* delay slot for load */
	/* Now, restore the registers */
	andi    $s0, $s0, 0xff00 /* preserve interrupt mask */
	LONG_L  $s1, CTX_CR0_STAT($a1)
	andi    $s1, $s1, 0x00ff /* load only status bits */
	or      $s0, $s0, $s1 /* combine them together */
	mtc0    $s0, $12
	LONG_L  $s0, CTX_S0($a1)
	LONG_L  $s1, CTX_S1($a1)
	LONG_L  $s2, CTX_S2($a1)
	LONG_L  $s3, CTX_S3($a1)
	LONG_L  $s4, CTX_S4($a1)
	LONG_L  $s5, CTX_S5($a1)
	LONG_L  $s6, CTX_S6($a1)
	LONG_L  $s7, CTX_S7($a1)
	LONG_L  $s8, CTX_S8($a1)
	LONG_L  $gp, CTX_GP($a1)
	LONG_L  $ra, CTX_RA($a1)
	nop     /* delay slot for load */
	LONG_ADDI $sp, $sp, CTX_SIZE
	/* and return. */
	jr      $ra
	nop     /* in delay slot */
END(context_switch)

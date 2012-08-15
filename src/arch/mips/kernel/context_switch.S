/*
 * @file
 *
 * @date Aug 15, 2012
 * @author: Anton Bondarev
 */

#include <asm/linkage.h>

/* a0/a1 points to old/new thread’s control block */
/* Allocate stack space for saving 11 registers. 11*4 = 44 */
C_ENTRY(contex_switch):
	addi $sp, $sp, -44
	/* Save the registers */
	sw $ra, 40($sp)
	sw $gp, 36($sp)
	sw $s8, 32($sp)
	sw $s7, 28($sp)
	sw $s6, 24($sp)
	sw $s5, 20($sp)
	sw $s4, 16($sp)
	sw $s3, 12($sp)
	sw $s2, 8($sp)
	sw $s1, 4($sp)
	sw $s0, 0($sp)
	/* Store the old stack pointer in the old control block */
	sw $sp, 0($a0)
	/* Get the new stack pointer from the new control block */
	lw $sp, 0($a1)
	nop /* delay slot for load */
	/* Now, restore the registers */
	lw $s0, 0($sp)
	lw $s1, 4($sp)
	lw $s2, 8($sp)
	lw $s3, 12($sp)
	lw $s4, 16($sp)
	lw $s5, 20($sp)
	lw $s6, 24($sp)
	lw $s7, 28($sp)
	lw $s8, 32($sp)
	lw $gp, 36($sp)
	lw $ra, 40($sp)
	nop /* delay slot for load */
	j $ra
	/* and return. */
	addi $sp, $sp, 44 /* in delay slot */

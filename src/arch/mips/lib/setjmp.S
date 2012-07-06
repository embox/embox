/**
 * @file
 *
 * @date 02.11.09
 * @author Anton Bondarev
 */

#include <asm/linkage.h>


	.section ".text"
/* a0 conteinst pointer to the setjmp buffer */
C_ENTRY(setjmp):
	sw   $s0, (0 * 4) ($a0)
	sw   $s1, (1 * 4) ($a0)
	sw   $s2, (2 * 4) ($a0)
	sw   $s3, (3 * 4) ($a0)
	sw   $s4, (4 * 4) ($a0)
	sw   $s5, (5 * 4) ($a0)
	sw   $s6, (6 * 4) ($a0)
	sw   $s7, (7 * 4) ($a0)
	sw   $s8, (8 * 4) ($a0)    /* frame pointer */
	sw   $ra, (9 * 4) ($a0)    /* return address */
	sw   $gp, (10 * 4) ($a0)   /* global data pointer */
	sw   $sp, (11 * 4) ($a0)   /* stack pointer */

	move $v0, $zero            /* return zero */
	j    $ra
	nop



C_ENTRY(longjmp):
	lw   $s0, (0 * 4) ($a0)  /* restore register variables s0-s8 */
	lw   $s1, (1 * 4) ($a0)
	lw   $s2, (2 * 4) ($a0)
	lw   $s3, (3 * 4) ($a0)
	lw   $s4, (4 * 4) ($a0)
	lw   $s5, (5 * 4) ($a0)
	lw   $s6, (6 * 4) ($a0)
	lw   $s7, (7 * 4) ($a0)
	lw   $s8, (8 * 4) ($a0)  /* frame pointer */
	lw   $ra, (9 * 4) ($a0)  /* return address */
	lw   $gp, (10 * 4) ($a0) /* global data pointer */
	lw   $sp, (11 * 4) ($a0) /* stack pointer */

	/* checking whether second argument is zero */
	bnez $a1, 1f
	nop
	addi $a1, $a1, 1  /* we must return 1 if val=0 */
1:
	move $v0, $a1 /* return value of second argument */
	j    $ra
	nop



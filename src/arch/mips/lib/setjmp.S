/**
 * @file
 *
 * @date 02.11.09
 * @author Anton Bondarev
 */

#include <asm/asm.h>
#include <asm/setjmp.h>


	.section .text
/* a0 conteinst pointer to the setjmp buffer */
LEAF(setjmp)
	sw   $s0, (SETJMP_S0) ($a0)    /* save static registers s0-s8 */
	sw   $s1, (SETJMP_S1) ($a0)
	sw   $s2, (SETJMP_S2) ($a0)
	sw   $s3, (SETJMP_S3) ($a0)
	sw   $s4, (SETJMP_S4) ($a0)
	sw   $s5, (SETJMP_S5) ($a0)
	sw   $s6, (SETJMP_S6) ($a0)
	sw   $s7, (SETJMP_S7) ($a0)
	sw   $s8, (SETJMP_FP) ($a0)    /* frame pointer */
	sw   $ra, (SETJMP_RA) ($a0)    /* return address */
	sw   $gp, (SETJMP_GP) ($a0)    /* global data pointer */
	sw   $sp, (SETJMP_SP) ($a0)    /* stack pointer */

	move $v0, $zero                /* return zero */
	j    $ra
	nop
END(setjmp)


/* a0 - setjmp buffer
 * a1 - return value
 */
LEAF(longjmp)
	lw   $s0, (SETJMP_S0) ($a0)  /* restore static registers s0-s8 */
	lw   $s1, (SETJMP_S1) ($a0)
	lw   $s2, (SETJMP_S2) ($a0)
	lw   $s3, (SETJMP_S3) ($a0)
	lw   $s4, (SETJMP_S4) ($a0)
	lw   $s5, (SETJMP_S5) ($a0)
	lw   $s6, (SETJMP_S6) ($a0)
	lw   $s7, (SETJMP_S7) ($a0)
	lw   $s8, (SETJMP_FP) ($a0)  /* frame pointer */
	lw   $ra, (SETJMP_RA) ($a0)  /* return address */
	lw   $gp, (SETJMP_GP) ($a0)  /* global data pointer */
	lw   $sp, (SETJMP_SP) ($a0)  /* stack pointer */

	/* checking whether second argument is zero */
	bnez $a1, 1f
	nop
	addi $a1, $a1, 1         /* we must return 1 if val=0 */
1:
	move $v0, $a1            /* return value of second argument */
	j    $ra
END(longjmp)

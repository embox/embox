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
	LONG_S  $s0, (SETJMP_S0) ($a0) /* save static registers s0-s8 */
	LONG_S  $s1, (SETJMP_S1) ($a0)
	LONG_S  $s2, (SETJMP_S2) ($a0)
	LONG_S  $s3, (SETJMP_S3) ($a0)
	LONG_S  $s4, (SETJMP_S4) ($a0)
	LONG_S  $s5, (SETJMP_S5) ($a0)
	LONG_S  $s6, (SETJMP_S6) ($a0)
	LONG_S  $s7, (SETJMP_S7) ($a0)
	LONG_S  $s8, (SETJMP_FP) ($a0) /* frame pointer */
	LONG_S  $ra, (SETJMP_RA) ($a0) /* return address */
	LONG_S  $gp, (SETJMP_GP) ($a0) /* global data pointer */
	LONG_S  $sp, (SETJMP_SP) ($a0) /* stack pointer */

	move    $v0, $zero /* return zero */
	j       $ra
	nop
END(setjmp)

/**
 * a0 - setjmp buffer
 * a1 - return value
 */
LEAF(longjmp)
	LONG_L  $s0, (SETJMP_S0) ($a0) /* restore static registers s0-s8 */
	LONG_L  $s1, (SETJMP_S1) ($a0)
	LONG_L  $s2, (SETJMP_S2) ($a0)
	LONG_L  $s3, (SETJMP_S3) ($a0)
	LONG_L  $s4, (SETJMP_S4) ($a0)
	LONG_L  $s5, (SETJMP_S5) ($a0)
	LONG_L  $s6, (SETJMP_S6) ($a0)
	LONG_L  $s7, (SETJMP_S7) ($a0)
	LONG_L  $s8, (SETJMP_FP) ($a0) /* frame pointer */
	LONG_L  $ra, (SETJMP_RA) ($a0) /* return address */
	LONG_L  $gp, (SETJMP_GP) ($a0) /* global data pointer */
	LONG_L  $sp, (SETJMP_SP) ($a0) /* stack pointer */

	/* checking whether second argument is zero */
	bnez    $a1, 1f
	nop
	LONG_ADDI $a1, $a1, 1 /* we must return 1 if val=0 */
1:
	move    $v0, $a1 /* return value of second argument */
	j       $ra
END(longjmp)

/*
 * @file
 *
 * @date Oct 17, 2012
 * @author: Anton Bondarev
 */

#include <asm/linkage.h>

C_ENTRY(setjmp)
	mflr	%r11			/* save return address */
	mfcr	%r12			/* save condition register */
	mr	%r10,%r1		/* save stack pointer */
	mr	%r9,%r2			/* save GPR2 (not needed) */
	stmw	%r9,8(%r3)		/* save r9..r31 */
	li	%r3,0			/* indicate success */
	blr				/* return */

C_ENTRY(longjmp)
	lmw	%r9,8(%r3)		/* save r9..r31 */
	mtlr	%r11			/* restore LR */
	mtcr	%r12			/* restore CR */
	mr	%r2,%r9			/* restore GPR2 (not needed) */
	mr	%r1,%r10		/* restore stack */
	or.	%r3,%r4,%r4		/* get return value */
	bnelr				/* return if not 0 */
	li	%r3,1			/* what's the point? */
	blr				/* return */

/**
 * @file
 *
 * @date 02.11.09
 * @author Anton Bondarev
 */

#include <asm/linkage.h>
#include <asm/setjmp.h>

	.section ".text"

C_ENTRY(setjmp):
	/* Save registers relative to r5 (arg0)*/
	swi	r1, r5, JB_SP             /* stack pointer */
	swi	r15, r5, JB_LP            /* link register */
	swi	r18, r5, JB_REGS + 0x00   /* Callee-saved registers r18-r30 */
	swi	r19, r5, JB_REGS + 0x04
	swi	r20, r5, JB_REGS + 0x08
	swi	r21, r5, JB_REGS + 0x0C
	swi	r22, r5, JB_REGS + 0x10
	swi	r23, r5, JB_REGS + 0x14
	swi	r24, r5, JB_REGS + 0x18
	swi	r25, r5, JB_REGS + 0x1C
	swi	r26, r5, JB_REGS + 0x20
	swi	r27, r5, JB_REGS + 0x24
	swi	r28, r5, JB_REGS + 0x28
	swi	r29, r5, JB_REGS + 0x2C
	swi	r30, r5, JB_REGS + 0x30

	addi	r3, r0, 0		/* return val */
	rtsd	r15, 8			/* normal return */
	nop

C_ENTRY(longjmp):
	/* load registers from memory to r5 (arg0)*/
	lwi	r1, r5, JB_SP
	lwi	r15, r5, JB_LP
	lwi	r18, r5, JB_REGS + 0x00
	lwi	r19, r5, JB_REGS + 0x04
	lwi	r20, r5, JB_REGS + 0x08
	lwi	r21, r5, JB_REGS + 0x0C
	lwi	r22, r5, JB_REGS + 0x10
	lwi	r23, r5, JB_REGS + 0x14
	lwi	r24, r5, JB_REGS + 0x18
	lwi	r25, r5, JB_REGS + 0x1C
	lwi	r26, r5, JB_REGS + 0x20
	lwi	r27, r5, JB_REGS + 0x24
	lwi	r28, r5, JB_REGS + 0x28
	lwi	r29, r5, JB_REGS + 0x2C
	lwi	r30, r5, JB_REGS + 0x30

	beqi r6, sec_arg_zero   /* branch if not equal to zero*/
	addi	r3, r6, 0		/* return val */
	rtsd	r15, 8			/* normal return */
	nop

sec_arg_zero:
	addi	r3, r0, 1		/* return val */
	rtsd	r15, 8			/* normal return */
	nop

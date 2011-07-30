/**
 * @file
 *
 * @brief
 *
 * @date 29.07.2011
 * @author Anton Bondarev
 */

#include <asm/linkage.h>
#include <hal/context.h>

/* r5 prev_context, r6 next context and we use r11 as temporary*/
C_ENTRY(context_switch):
/*
 * SAVE PREVIOUS CONTEXT
 */
	swi	r1, r5, CC_R1
	swi	r2, r5, CC_R2
	/* skip volatile registers.
	 * they are already saved on stack */
	/* dedicated registers */
	swi	r13, r5, CC_R13
	swi	r14, r5, CC_R14
	swi	r15, r5, CC_R15
	swi	r16, r5, CC_R16
	swi	r17, r5, CC_R17
	swi	r18, r5, CC_R18
	/* save non-volatile registers */
	swi	r19, r5, CC_R19
	swi	r20, r5, CC_R20
	swi	r21, r5, CC_R21
	swi	r22, r5, CC_R22
	swi	r23, r5, CC_R23
	swi	r24, r5, CC_R24
	swi	r25, r5, CC_R25
	swi	r26, r5, CC_R26
	swi	r27, r5, CC_R27
	swi	r28, r5, CC_R28
	swi	r29, r5, CC_R29
	swi	r30, r5, CC_R30
	swi r31, r5, CC_R31

	/* special purpose registers */
	mfs	r11, rmsr
	swi	r11, r5, CC_RMSR
	mfs	r11, rear
	swi	r11, r5, CC_REAR
	mfs	r11, resr
	swi	r11, r5, CC_RESR
	mfs	r11, rfsr
	swi	r11, r5, CC_RFSR
/*
 * LOAD NEW CONTEXT
 */
	/* special purpose registers */
	lwi	r11, r6, CC_RFSR
	mts	rfsr, r11
	lwi	r11, r6, CC_RESR
	mts	resr, r11
	lwi	r11, r6, CC_REAR
	mts	rear, r11
	lwi	r11, r6, CC_RMSR
	mts	rmsr, r11
	/* non-volatile registers */
	lwi	r30, r6, CC_R30
	lwi	r29, r6, CC_R29
	lwi	r28, r6, CC_R28
	lwi	r27, r6, CC_R27
	lwi	r26, r6, CC_R26
	lwi	r25, r6, CC_R25
	lwi	r24, r6, CC_R24
	lwi	r23, r6, CC_R23
	lwi	r22, r6, CC_R22
	lwi	r21, r6, CC_R21
	lwi	r20, r6, CC_R20
	lwi	r19, r6, CC_R19
	/* dedicated registers */
	lwi	r18, r6, CC_R18
	lwi	r17, r6, CC_R17
	lwi	r16, r6, CC_R16
	lwi	r15, r6, CC_R15
	lwi	r14, r6, CC_R14
	lwi	r13, r6, CC_R13
	/* skip volatile registers */
	lwi	r2, r6, CC_R2
	lwi	r1, r6, CC_R1

	rtsd	r15, 8
	 nop

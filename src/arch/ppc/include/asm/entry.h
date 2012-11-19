/**
 * @file
 *
 * @date 14.11.12
 * @author Ilia Vaprol
 */

#ifndef PPC_ENTRY_H_
#define PPC_ENTRY_H_

#ifdef __ASSEMBLER__

/**
 * Save all registers on stack:
 * +---------------+
 * | 0x00 - GPR0   | General Purpose Register 0
 * +---------------+
 * | 0x04 - GPR2   | General Purpose Register 2
 * +---------------+
 * |     ...       |
 * +---------------+
 * | 0x7C - GPR31  | General Purpose Register 31
 * +---------------+
 * | 0x80 - LR     | Link Register
 * +---------------+
 * | 0x84 - CR     | Condition Register
 * +---------------+
 * | 0x88 - XER    | Integer Exception Register
 * +---------------+
 * | 0x8C - CTR    | Count Register
 * +---------------+
 * | 0x90 - SRR0   | Save/Restore Register 0
 * +---------------+
 * | 0x94 - SRR1   | Save/Restore Register 1
 * +---------------+
 */

	.macro SAVE_ALL
		subi   r1, r1, 0x98  /* allocate space to save all registers */
		stw    r0, 0x00(r1)  /* save r0 */
		stmw   r2, 0x04(r1)  /* save r2..r31 */
		mflr   r26           /* save lr, cr, xer, ctr, srr0, srr1 */
		mfcr   r27
		mfxer  r28
		mfctr  r29
		mfsrr0 r30
		mfsrr1 r31
		stmw   r26, 0x80(r1)
	.endm

	.macro RESTORE_ALL
		lmw    r26, 0x80(r1) /* restore lr, cr, xer, ctr, srr0, srr1 */
		mtlr   r26
		mtcr   r27
		mtxer  r28
		mtctr  r29
		mtsrr0 r30
		mtsrr1 r31
		lmw    r2, 0x04(r1)  /* restore r2..r31 */
		lwz    r0, 0x00(r1)  /* restore r0 */
		addi   r1, r1, 0x98 /* get back stack */
	.endm

#endif /* __ASSEMBLER__ */

#endif /* PPC_ENTRY_H_ */

/**
 * @file
 *
 * @date 14.11.12
 * @author Ilia Vaprol
 */

#ifndef PPC_ENTRY_H_
#define PPC_ENTRY_H_

#ifdef __ASSEMBLER__
	.macro SAVE_ALL
	    subis r1, r1, 132@ha /* alloc mem for registers */
		subi  r1, r1, 132@l
		stw   r0, 0(r1)      /* save r0 */
	    mflr  r0             /* save lr */
		stw   r0, 4(r1)
	    mfcr  r0             /* save cr */
		stw   r0, 8(r1)
		stmw  r3, 12(r1)     /* save r2..r31 */
	.endm

	.macro RESTORE_ALL
		lmw   r3, 12(r1)     /* restore r2..r31 */
		lwz   r0, 8(r1)      /* restore cr */
		mtcr  r0
		lwz   r0, 4(r1)      /* restore lr */
	    mtlr  r0
		lwz   r0, 0(r1)      /* restore r0 */
	    addis r1, r1, 132@ha /* free mem for registers */
		addi  r1, r1, 132@l
	.endm

#endif /* __ASSEMBLER__ */

#endif /* PPC_ENTRY_H_ */

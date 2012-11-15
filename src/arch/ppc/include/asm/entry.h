/**
 * @file
 *
 * @date 14.11.12
 * @author Ilia Vaprol
 */

#ifndef PPC_ENTRY_H_
#define PPC_ENTRY_H_

#define REG_GPR0  0x00
#define REG_GPR1  0x04
#define REG_GPR2  0x08
#define REGS_GPR_2_31  REG_GPR2
#define REG_GPR3  0x0c
#define REG_GPR4  0x10
#define REG_GPR5  0x14
#define REG_GPR6  0x18
#define REG_GPR7  0x1c
#define REG_GPR8  0x20
#define REG_GPR9  0x24
#define REG_GPR10 0x28
#define REG_GPR11 0x2c
#define REG_GPR12 0x30
#define REG_GPR13 0x34
#define REG_GPR14 0x38
#define REG_GPR15 0x3c
#define REG_GPR16 0x40
#define REG_GPR17 0x44
#define REG_GPR18 0x48
#define REG_GPR19 0x4c
#define REG_GPR20 0x50
#define REG_GPR21 0x54
#define REG_GPR22 0x58
#define REG_GPR23 0x5c
#define REG_GPR24 0x60
#define REG_GPR25 0x64
#define REG_GPR26 0x68
#define REG_GPR27 0x6c
#define REG_GPR28 0x70
#define REG_GPR29 0x74
#define REG_GPR30 0x78
#define REG_GPR31 0x7c
#define REG_LR    0x80
#define REGS_LR_CR_XER_CTR_SRR0_SRR1 REG_LR
#define REG_CR    0x84
#define REG_XER   0x88
#define REG_CTR   0x8c
#define REG_SRR0  0x90
#define REG_SRR1  0x94
//#define CTX_TRAPNO     0x98
#define CTXREGS        (4*39)


#ifdef __ASSEMBLER__

	.macro SAVE_ALL
		subis  r1, r1, CTXREGS@ha    /* alloc space for context registers */
        subi   r1, r1, CTXREGS@l
		stw    r0, REG_GPR0(r1)      /* save r0 */
        stmw   r2, REGS_GPR_2_31(r1) /* save r2..r31 */
        mflr   r26                   /* save lr, cr, xer, ctr, srr0, srr1 */
        mfcr   r27
        mfxer  r28
        mfctr  r29
        mfsrr0 r30
        mfsrr1 r31
        stmw   r26, REGS_LR_CR_XER_CTR_SRR0_SRR1(r1)
	.endm

	.macro RESTORE_ALL
        lmw   r26, REGS_LR_CR_XER_CTR_SRR0_SRR1(r1) /* restore lr, cr, xer, ctr, srr0, srr1 */
        mtlr   r26
        mtcr   r27
        mtxer  r28
        mtctr  r29
        mtsrr0 r30
        mtsrr1 r31
        lmw    r2, REGS_GPR_2_31(r1) /* restore r2..r31 */
        lwz    r0, REG_GPR0(r1)      /* restore r0 */
        addis   r1, r1, CTXREGS@ha   /* free memory from context registers */
        addi    r1, r1, CTXREGS@l
	.endm

#endif /* __ASSEMBLER__ */

#endif /* PPC_ENTRY_H_ */

/**
 * @file
 *
 * @date 08.11.12
 * @author Ilia Vaprol
 */

#ifndef PPC_TRAPS_H_
#define PPC_TRAPS_H_

#ifndef __ASSEMBLER__

typedef struct pt_regs {
	unsigned int gpr[32]; /* General Purpose Registers */
	unsigned int lr;      /* Link Register */
	unsigned int cr;      /* Condition Register */
	unsigned int xer;     /* XER Register */
	unsigned int ctr;     /* Count Register */
	unsigned int srr0;    /* Machine Status Save/Restore Register 0 */
	unsigned int srr1;    /* Machine Status Save/Restore Register 1 */
	unsigned int trapno;  /* Type of interrupt */
} pt_regs_t;

#else

#define PT_REGS_GPR0   0x00
#define PT_REGS_GPR1   0x04
#define PT_REGS_GPR2   0x08
#define PT_REGS_GPR3   0x0C
#define PT_REGS_GPR4   0x10
#define PT_REGS_GPR5   0x14
#define PT_REGS_GPR6   0x18
#define PT_REGS_GPR7   0x1C
#define PT_REGS_GPR8   0x20
#define PT_REGS_GPR9   0x24
#define PT_REGS_GPR10  0x28
#define PT_REGS_GPR11  0x2C
#define PT_REGS_GPR12  0x30
#define PT_REGS_GPR13  0x34
#define PT_REGS_GPR14  0x38
#define PT_REGS_GPR15  0x3C
#define PT_REGS_GPR16  0x40
#define PT_REGS_GPR17  0x44
#define PT_REGS_GPR18  0x48
#define PT_REGS_GPR19  0x4C
#define PT_REGS_GPR20  0x50
#define PT_REGS_GPR21  0x54
#define PT_REGS_GPR22  0x58
#define PT_REGS_GPR23  0x5C
#define PT_REGS_GPR24  0x60
#define PT_REGS_GPR25  0x64
#define PT_REGS_GPR26  0x68
#define PT_REGS_GPR27  0x6C
#define PT_REGS_GPR28  0x70
#define PT_REGS_GPR29  0x74
#define PT_REGS_GPR30  0x78
#define PT_REGS_GPR31  0x7C
#define PT_REGS_LR     0x80
#define PT_REGS_CR     0x84
#define PT_REGS_XER    0x88
#define PT_REGS_CTR    0x8C
#define PT_REGS_SRR0   0x90
#define PT_REGS_SRR1   0x94
#define PT_REGS_TRAPNO 0x98

#define PT_REGS_SIZE   0x9C

	.macro SAVE_ALL
		subi   r1, r1, PT_REGS_SIZE  /* allocate space to save all registers */
		stw    r0, PT_REGS_GPR0(r1)  /* save r0, r2..r31 */
		stmw   r2, PT_REGS_GPR2(r1)
		mflr   r26                   /* save lr, cr, xer, ctr, srr0, srr1 */
		mfcr   r27
		mfxer  r28
		mfctr  r29
		mfsrr0 r30
		mfsrr1 r31
		stmw   r26, PT_REGS_LR(r1)
		addi   r0, r1, PT_REGS_SIZE  /* save original r1 */
		stw    r0, PT_REGS_GPR1(r1)
	.endm

	.macro RESTORE_ALL
		lmw    r26, PT_REGS_LR(r1) /* restore lr, cr, xer, ctr, srr0, srr1 */
		mtlr   r26
		mtcr   r27
		mtxer  r28
		mtctr  r29
		mtsrr0 r30
		mtsrr1 r31
		lwz    r0, PT_REGS_GPR0(r1) /* restore r0, r2..r31 */
		lmw    r2, PT_REGS_GPR2(r1)
		lwz    r1, PT_REGS_GPR1(r1) /* restore old stack into r1 */
	.endm

#endif /* __ASSEMBLER__ */

#endif /* PPC_TRAPS_H_ */

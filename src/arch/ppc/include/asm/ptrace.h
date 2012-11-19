/**
 * @file
 *
 * @date 19.11.12
 * @author Ilia Vaprol
 */

#ifndef PPC_PTRACE_H_
#define PPC_PTRACE_H_

#ifndef __ASSEMBLER__

typedef struct pt_regs {
	unsigned int gpr[32]; /* General Purpose Registers */
	unsigned int lr;      /* Link Register */
	unsigned int cr;      /* Condition Register */
	unsigned int xer;     /* XER Register */
	unsigned int ctr;     /* Count Register */
	unsigned int srr0;    /* Machine Status Save/Restore Register 0 */
	unsigned int srr1;    /* Machine Status Save/Restore Register 1 */
} pt_regs_t;

#else /* __ASSEMBLER__ */

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

#define PT_REGS_SIZE   0x98

	.macro PT_REGS_SAVE
		subi   r1, r1, PT_REGS_SIZE   /* allocate space */
		stw    r0, PT_REGS_GPR0(r1)   /* save r0, r2..r31 */
		stw    r2, PT_REGS_GPR2(r1)
		stw    r3, PT_REGS_GPR3(r1)
		stw    r4, PT_REGS_GPR4(r1)
		stw    r5, PT_REGS_GPR5(r1)
		stw    r6, PT_REGS_GPR6(r1)
		stw    r7, PT_REGS_GPR7(r1)
		stw    r8, PT_REGS_GPR8(r1)
		stw    r9, PT_REGS_GPR9(r1)
		stw    r10, PT_REGS_GPR10(r1)
		stw    r11, PT_REGS_GPR11(r1)
		stw    r12, PT_REGS_GPR12(r1)
		stw    r13, PT_REGS_GPR13(r1)
		stw    r14, PT_REGS_GPR14(r1)
		stw    r15, PT_REGS_GPR15(r1)
		stw    r16, PT_REGS_GPR16(r1)
		stw    r17, PT_REGS_GPR17(r1)
		stw    r18, PT_REGS_GPR18(r1)
		stw    r19, PT_REGS_GPR19(r1)
		stw    r20, PT_REGS_GPR20(r1)
		stw    r21, PT_REGS_GPR21(r1)
		stw    r22, PT_REGS_GPR22(r1)
		stw    r23, PT_REGS_GPR23(r1)
		stw    r24, PT_REGS_GPR24(r1)
		stw    r25, PT_REGS_GPR25(r1)
		stw    r26, PT_REGS_GPR26(r1)
		stw    r27, PT_REGS_GPR27(r1)
		stw    r28, PT_REGS_GPR28(r1)
		stw    r29, PT_REGS_GPR29(r1)
		stw    r30, PT_REGS_GPR30(r1)
		stw    r31, PT_REGS_GPR31(r1)
		mflr   r0                     /* save lr */
		stw    r0, PT_REGS_LR(r1)
		mfcr   r0                     /* save cr */
		stw    r0, PT_REGS_CR(r1)
		mfxer  r0                     /* save xer */
		stw    r0, PT_REGS_XER(r1)
		mfctr  r0                     /* save ctr */
		stw    r0, PT_REGS_CTR(r1)
		mfsrr0 r0                     /* save srr0 */
		stw    r0, PT_REGS_SRR0(r1)
		mfsrr1 r0                     /* save srr1 */
		stw    r0, PT_REGS_SRR1(r1)
		addi   r0, r1, PT_REGS_SIZE   /* get previous stack pointer and save into r1 */
		stw    r0, PT_REGS_GPR1(r1)
	.endm

	.macro PT_REGS_RESTORE
		lwz    r0, PT_REGS_LR(r1)     /* restore lr */
		mtlr   r0
		lwz    r0, PT_REGS_CR(r1)     /* restore cr */
		mtcr   r0
		lwz    r0, PT_REGS_XER(r1)    /* restore xer */
		mtxer  r0
		lwz    r0, PT_REGS_CTR(r1)    /* restore ctr */
		mtctr  r0
		lwz    r0, PT_REGS_SRR0(r1)   /* restore srr0 */
		mtsrr0 r0
		lwz    r0, PT_REGS_SRR1(r1)   /* restore srr1 */
		mtsrr1 r0
		lwz    r0, PT_REGS_GPR0(r1)   /* restore r0, r2..r31 */
		lwz    r2, PT_REGS_GPR2(r1)
		lwz    r3, PT_REGS_GPR3(r1)
		lwz    r4, PT_REGS_GPR4(r1)
		lwz    r5, PT_REGS_GPR5(r1)
		lwz    r6, PT_REGS_GPR6(r1)
		lwz    r7, PT_REGS_GPR7(r1)
		lwz    r8, PT_REGS_GPR8(r1)
		lwz    r9, PT_REGS_GPR9(r1)
		lwz    r10, PT_REGS_GPR10(r1)
		lwz    r11, PT_REGS_GPR11(r1)
		lwz    r12, PT_REGS_GPR12(r1)
		lwz    r13, PT_REGS_GPR13(r1)
		lwz    r14, PT_REGS_GPR14(r1)
		lwz    r15, PT_REGS_GPR15(r1)
		lwz    r16, PT_REGS_GPR16(r1)
		lwz    r17, PT_REGS_GPR17(r1)
		lwz    r18, PT_REGS_GPR18(r1)
		lwz    r19, PT_REGS_GPR19(r1)
		lwz    r20, PT_REGS_GPR20(r1)
		lwz    r21, PT_REGS_GPR21(r1)
		lwz    r22, PT_REGS_GPR22(r1)
		lwz    r23, PT_REGS_GPR23(r1)
		lwz    r24, PT_REGS_GPR24(r1)
		lwz    r25, PT_REGS_GPR25(r1)
		lwz    r26, PT_REGS_GPR26(r1)
		lwz    r27, PT_REGS_GPR27(r1)
		lwz    r28, PT_REGS_GPR28(r1)
		lwz    r29, PT_REGS_GPR29(r1)
		lwz    r30, PT_REGS_GPR30(r1)
		lwz    r31, PT_REGS_GPR31(r1)
		lwz    r1, PT_REGS_GPR1(r1)   /* restore old stack */
	.endm

	.macro PTRACE_DEBUG
		.globl ptrace_info
		PT_REGS_SAVE
		mr r3, r1
		subi r1, r1, 8                /* alloc space for stack pointer and link register */
		bl ptrace_info
		addi r1, r1, 8                /* free space */
		PT_REGS_RESTORE
	.endm

#endif /* !__ASSEMBLER__ */

#endif /* PPC_PTRACE_H_ */

/**
 * @file
 *
 * @date 23.07.10
 * @author Avdyukhin Dmitry
 */

#include <asm/linkage.h>
#include <asm/setjmp.h>

	.section ".text"

/* offsets in pt_regs_t structure */
#define PT_R0   0x00
#define PT_R1   0x04
#define PT_R2   0x08
#define PT_R3   0x0C
#define PT_R4   0x10
#define PT_R5   0x14
#define PT_R6   0x18
#define PT_R7   0x1C
#define PT_R8   0x20
#define PT_R9   0x24
#define PT_R10  0x28
#define PT_R11  0x2C
#define PT_R12  0x30
#define PT_R13  0x34
#define PT_R14  0x38
#define PT_R15  0x3C
#define PT_R16  0x40
#define PT_R17  0x44
#define PT_R18  0x48
#define PT_R19  0x4C
#define PT_R20  0x50
#define PT_R21  0x54
#define PT_R22  0x58
#define PT_R23  0x5C
#define PT_R24  0x60
#define PT_R25  0x64
#define PT_R26  0x68
#define PT_R27  0x6C
#define PT_R28  0x70
#define PT_R29  0x74
#define PT_R30  0x78
#define PT_R31  0x7C
#define PT_PC   0x80
#define PT_MSR  0x84
#define PT_EAR  0x88
#define PT_ESR  0x8C
#define PT_FSR  0x90

/* sizeof(pt_regs_t */
#define PT_SIZE 0x94

/* PTO-PT offset. Stack frame may include some function arguments and we must
 * allocate memory for it. See microblaze reference manual
 */
#define PTO     0
#define STATE_SAVE_SIZE (PTO + PT_SIZE)

#define SAVE_CONTEXT_REGS \
	swi	r1, r5, PTO+PT_R1;     \
	swi	r2, r5, PTO+PT_R2;     \
	swi	r3, r5, PTO+PT_R3;     \
	swi	r4, r5, PTO+PT_R4;     \
	swi	r5, r5, PTO+PT_R5;     \
	swi	r6, r5, PTO+PT_R6;     \
	swi	r7, r5, PTO+PT_R7;     \
	swi	r8, r5, PTO+PT_R8;     \
	swi	r9, r5, PTO+PT_R9;     \
	swi	r10, r5, PTO+PT_R10;   \
	swi	r11, r5, PTO+PT_R11;   \
	swi	r12, r5, PTO+PT_R12;   \
	swi	r13, r5, PTO+PT_R13;   \
	swi	r14, r5, PTO+PT_R14;   \
	swi	r15, r5, PTO+PT_R15;   \
	swi	r18, r5, PTO+PT_R18;   \
	swi	r19, r5, PTO+PT_R19;   \
	swi	r20, r5, PTO+PT_R20;   \
	swi	r21, r5, PTO+PT_R21;   \
	swi	r22, r5, PTO+PT_R22;   \
	swi	r23, r5, PTO+PT_R23;   \
	swi	r24, r5, PTO+PT_R24;   \
	swi	r25, r5, PTO+PT_R25;   \
	swi	r26, r5, PTO+PT_R26;   \
	swi	r27, r5, PTO+PT_R27;   \
	swi	r28, r5, PTO+PT_R28;   \
	swi	r29, r5, PTO+PT_R29;   \
	swi	r30, r5, PTO+PT_R30;   \
	swi	r31, r5, PTO+PT_R31;   \
	mfs	r11, rmsr;             \
	nop;                       \
	swi	r11, r5, PTO+PT_MSR;

/* restore processor registers */
#define RESTORE_CONTEXT_REGS \
	lwi	r11, r5, PTO+PT_MSR;    \
	mts	rmsr , r11;             \
	nop;                        \
	lwi	r1, r5, PTO+PT_R1;      \
	lwi	r2, r5, PTO+PT_R2;      \
	\
	lwi	r4, r5, PTO+PT_R4;      \
	\
	\
	lwi	r7, r5, PTO+PT_R7;      \
	lwi	r8, r5, PTO+PT_R8;      \
	lwi	r9, r5, PTO+PT_R9;      \
	lwi	r10, r5, PTO+PT_R10;    \
	lwi	r11, r5, PTO+PT_R11;    \
	lwi	r12, r5, PTO+PT_R12;    \
	lwi	r13, r5, PTO+PT_R13;    \
	lwi	r14, r5, PTO+PT_R14;    \
	lwi	r15, r5, PTO+PT_R15;    \
	lwi	r18, r5, PTO+PT_R18;    \
	lwi	r19, r5, PTO+PT_R19;    \
	lwi	r20, r5, PTO+PT_R20;    \
	lwi	r21, r5, PTO+PT_R21;    \
	lwi	r22, r5, PTO+PT_R22;    \
	lwi	r23, r5, PTO+PT_R23;    \
	lwi	r24, r5, PTO+PT_R24;    \
	lwi	r25, r5, PTO+PT_R25;    \
	lwi	r26, r5, PTO+PT_R26;    \
	lwi	r27, r5, PTO+PT_R27;    \
	lwi	r28, r5, PTO+PT_R28;    \
	lwi	r29, r5, PTO+PT_R29;    \
	lwi	r30, r5, PTO+PT_R30;    \
	lwi	r31, r5, PTO+PT_R31;    \

C_ENTRY(setjmp):

	/* Save registers relative to r5 (arg0)*/
	SAVE_CONTEXT_REGS
	addi	r3, r0, 0		/* return val */
	rtsd	r15, 8			/* normal return */
	nop

C_ENTRY(longjmp):
	/* load registers from memory to r5 (arg0)*/
	RESTORE_CONTEXT_REGS
	beqi r6, sec_arg_zero   /* branch if not equal to zero*/
	addi	r3, r6, 0		/* return val */
	lwi	r6, r5, PTO+PT_R6
	lwi	r5, r5, PTO+PT_R5
	rtsd	r15, 8			/* normal return */
	nop

sec_arg_zero:

	addi	r3, r0, 1		/* return val */
	lwi	r6, r5, PTO+PT_R6
	lwi	r5, r5, PTO+PT_R5
	rtsd	r15, 8			/* normal return */
	nop

/*
 * @file
 * @brief aliases for register's name mentioned in microblaze API
 * @data 17.11.2009
 * @author Anton Bondarev
 */

#ifndef MB_REGS_H_
#define MB_REGS_H_

#define reg_zero   r0    /**< zero constant register*/
#define reg_sp     r1    /**< stack pointer register*/
#define reg_tmp1   r11   /**< temporary register*/
#define reg_tmp2   r12   /**< temporary register*/
#define reg_ireta  r14   /**< return address from interrupt*/
#define reg_sreta  r15   /**< return address from subroutine*/
#define reg_treta  r16   /**< return address from trap*/
#define reg_ereta  r17   /**< return address from exception*/

#define REGS_INIT() \
	addi r2, reg_zero, 0;  \
	addi r5, reg_zero, 0;  \
	addi r6, reg_zero, 0;  \
	addi r7, reg_zero, 0;  \
	addi r8, reg_zero, 0;  \
	addi r9, reg_zero, 0;  \
	addi r10, reg_zero, 0; \
	addi r11, reg_zero, 0; \
	addi r12, reg_zero, 0; \
	addi r13, reg_zero, 0; \
	addi r14, reg_zero, 0; \
	addi r15, reg_zero, 0; \
	addi r18, reg_zero, 0; \
	addi r19, reg_zero, 0; \
	addi r20, reg_zero, 0; \
	addi r21, reg_zero, 0; \
	addi r22, reg_zero, 0; \
	addi r23, reg_zero, 0; \
	addi r24, reg_zero, 0; \
	addi r25, reg_zero, 0; \
	addi r26, reg_zero, 0; \
	addi r27, reg_zero, 0; \
	addi r28, reg_zero, 0; \
	addi r29, reg_zero, 0; \
	addi r30, reg_zero, 0; \
	addi r31, reg_zero, 0;
#ifndef __ASSEMBLER__

#include "types.h"

typedef struct stack_frame {
	uint32_t regs[32];
	uint32_t rmsr;
} stack_frame_t;

#endif /*__ASSEMBLER__*/

#define PTO 0 /*i don't still know why it must use*/

#define PT_R0  0x00
#define PT_R1  0x04
#define PT_R2  0x08
#define PT_R3  0x0C
#define PT_R4  0x10
#define PT_R5  0x14
#define PT_R6  0x18
#define PT_R7  0x1C
#define PT_R8  0x20
#define PT_R9  0x24
#define PT_R10 0x28
#define PT_R11 0x2C
#define PT_R12 0x30
#define PT_R13 0x34
#define PT_R14 0x38
#define PT_R15 0x3C
#define PT_R16 0x40
#define PT_R17 0x44
#define PT_R18 0x48
#define PT_R19 0x4C
#define PT_R20 0x50
#define PT_R21 0x54
#define PT_R22 0x58
#define PT_R23 0x5C
#define PT_R24 0x60
#define PT_R25 0x64
#define PT_R26 0x68
#define PT_R27 0x6C
#define PT_R28 0x70
#define PT_R29 0x74
#define PT_R30 0x78
#define PT_R31 0x7C

#define PT_MSR 0x80


#define SAVE_REGS \
	addi r1,r1, -(PT_MSR + 4); \
	swi	r2, r1, PTO+PT_R2;	/* Save SDA */			\
	swi	r5, r1, PTO+PT_R5;					\
	swi	r6, r1, PTO+PT_R6;					\
	swi	r7, r1, PTO+PT_R7;					\
	swi	r8, r1, PTO+PT_R8;					\
	swi	r9, r1, PTO+PT_R9;					\
	swi	r10, r1, PTO+PT_R10;					\
	swi	r11, r1, PTO+PT_R11;	/* save clobbered regs after rval */\
	swi	r12, r1, PTO+PT_R12;					\
	swi	r13, r1, PTO+PT_R13;	/* Save SDA2 */			\
	swi	r14, r1, PTO+PT_R14;	/* PC, before IRQ/trap */	\
	swi	r15, r1, PTO+PT_R15;	/* Save LP */			\
	swi	r18, r1, PTO+PT_R18;	/* Save asm scratch reg */	\
	swi	r19, r1, PTO+PT_R19;					\
	swi	r20, r1, PTO+PT_R20;					\
	swi	r21, r1, PTO+PT_R21;					\
	swi	r22, r1, PTO+PT_R22;					\
	swi	r23, r1, PTO+PT_R23;					\
	swi	r24, r1, PTO+PT_R24;					\
	swi	r25, r1, PTO+PT_R25;					\
	swi	r26, r1, PTO+PT_R26;					\
	swi	r27, r1, PTO+PT_R27;					\
	swi	r28, r1, PTO+PT_R28;					\
	swi	r29, r1, PTO+PT_R29;					\
	swi	r30, r1, PTO+PT_R30;					\
	swi	r31, r1, PTO+PT_R31;	/* Save current task reg */	\
	mfs	r11, rmsr;		/* save MSR */			\
	nop;								\
	swi	r11, r1, PTO+PT_MSR;

#define RESTORE_REGS \
	lwi	r11, r1, PTO+PT_MSR;					\
	mts	rmsr , r11;						\
	nop;								\
	lwi	r2, r1, PTO+PT_R2;	/* restore SDA */		\
	lwi	r5, r1, PTO+PT_R5;					\
	lwi	r6, r1, PTO+PT_R6;					\
	lwi	r7, r1, PTO+PT_R7;					\
	lwi	r8, r1, PTO+PT_R8;					\
	lwi	r9, r1, PTO+PT_R9;					\
	lwi	r10, r1, PTO+PT_R10;					\
	lwi	r11, r1, PTO+PT_R11;	/* restore clobbered regs after rval */\
	lwi	r12, r1, PTO+PT_R12;					\
	lwi	r13, r1, PTO+PT_R13;	/* restore SDA2 */		\
	lwi	r14, r1, PTO+PT_R14;	/* RESTORE_LINK PC, before IRQ/trap */\
	lwi	r15, r1, PTO+PT_R15;	/* restore LP */		\
	lwi	r18, r1, PTO+PT_R18;	/* restore asm scratch reg */	\
	lwi	r19, r1, PTO+PT_R19;					\
	lwi	r20, r1, PTO+PT_R20;					\
	lwi	r21, r1, PTO+PT_R21;					\
	lwi	r22, r1, PTO+PT_R22;					\
	lwi	r23, r1, PTO+PT_R23;					\
	lwi	r24, r1, PTO+PT_R24;					\
	lwi	r25, r1, PTO+PT_R25;					\
	lwi	r26, r1, PTO+PT_R26;					\
	lwi	r27, r1, PTO+PT_R27;					\
	lwi	r28, r1, PTO+PT_R28;					\
	lwi	r29, r1, PTO+PT_R29;					\
	lwi	r30, r1, PTO+PT_R30;					\
	lwi	r31, r1, PTO+PT_R31;	/* Restore cur task reg */ \
	addi r1,r1, (PT_MSR + 4);

#endif /* MB_REGS_H_ */

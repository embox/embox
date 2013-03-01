/**
 * @file
 * @brief
 *
 * @date 13.04.10
 * @author Eldar Abusalimov
 */

#ifndef ARCH_MICROBLAZE_CONTEXT_H_
#define ARCH_MICROBLAZE_CONTEXT_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

struct context {
	/* 0x00 */ uint32_t r1; /* stack pointer */
	/* 0x04 */ uint32_t r2;

	/* r3-r12 are volatile register (caller-save) */

	/* dedicated registers */
	/* 0x08 */ uint32_t r13;
	/* 0x0c */ uint32_t r14;
	/* 0x10 */ uint32_t r15;
	/* 0x14 */ uint32_t r16;
	/* 0x18 */ uint32_t r17;
	/* 0x1C */ uint32_t r18;

	/* non-volatile registers Callee-save */
	/* 0x20 */ uint32_t r19;
	/* 0x24 */ uint32_t r20;
	/* 0x28 */ uint32_t r21;
	/* 0x2C */ uint32_t r22;
	/* 0x30 */ uint32_t r23;
	/* 0x34 */ uint32_t r24;
	/* 0x38 */ uint32_t r25;
	/* 0x3C */ uint32_t r26;
	/* 0x40 */ uint32_t r27;
	/* 0x44 */ uint32_t r28;
	/* 0x48 */ uint32_t r29;
	/* 0x4C */ uint32_t r30;
	/* 0x50 */ uint32_t r31;

	/* special purpose registers */
	/* 0x50 */ uint32_t msr;
	/* 0x54 */ uint32_t ear;
	/* 0x58 */ uint32_t esr;
	/* 0x60 */ uint32_t fsr;
};
#endif /* __ASSEMBLER__ */

#define CC_R1      0x00
#define CC_R2      0x04
#define CC_R13     0x08
#define CC_R14     0x0C
#define CC_R15     0x10
#define CC_R16     0x14
#define CC_R17     0x18
#define CC_R18     0x1C
#define CC_R19     0x20
#define CC_R20     0x24
#define CC_R21     0x28
#define CC_R22     0x2C
#define CC_R23     0x30
#define CC_R24     0x34
#define CC_R25     0x38
#define CC_R26     0x3C
#define CC_R27     0x40
#define CC_R28     0x44
#define CC_R29     0x48
#define CC_R30     0x4C
#define CC_R31     0x50

#define CC_RMSR    0x54
#define CC_REAR    0x58
#define CC_RESR    0x5C
#define CC_RFSR    0x60

#endif /* ARCH_MICROBLAZE_CONTEXT_H_ */

/**
 * @file trap_regs.h
 *
 * @brief TODO
 *
 * @date 23.07.2009
 * @author Eldar Abusalimov
 */

#ifndef SPARC_TRAP_REGS_H_
#define SPARC_TRAP_REGS_H_

#include <types.h>
#include <asm/stack.h>

#ifndef __ASSEMBLER__

#define TRAP_REGS_SZ sizeof(struct trap_regs)

/**
 * Basic set of registers to save when trap occurs.
 */
struct trap_regs {
	uint32_t psr; /**< Processor state register. */
	uint32_t pc; /**< Program counter. */
	uint32_t npc; /**< Next Program counter. */
	uint32_t y; /**< MUL/DIV flags. */
	uint32_t globals[8]; /**< Global registers. */
	uint32_t ins[8]; /**< Input registers. */
}__attribute__ ((aligned (8)));

#else /* __ASSEMBLER__ */

/* compute sizes by hand (see above) */
#define TRAP_REGS_SZ ((1+1+1+1+8+8)*4)

/* Offsets for trap context structure  */
#define TR_PSR    0x0
#define TR_PC     0x4
#define TR_NPC    0x8
#define TR_Y      0xc
#define TR_G0     0x10
#define TR_G1     0x14
#define TR_G2     0x18
#define TR_G3     0x1c
#define TR_G4     0x20
#define TR_G5     0x24
#define TR_G6     0x28
#define TR_G7     0x2c
#define TR_I0     0x30
#define TR_I1     0x34
#define TR_I2     0x38
#define TR_I3     0x3c
#define TR_I4     0x40
#define TR_I5     0x44
#define TR_I6     0x48
#define TR_I7     0x4c

#define LOAD_TR_GLOBALS(base_reg) \
	ld  [%base_reg + STACK_FRAME_SZ + TR_G1], %g1; \
	ldd [%base_reg + STACK_FRAME_SZ + TR_G2], %g2; \
	ldd [%base_reg + STACK_FRAME_SZ + TR_G4], %g4; \
	ldd [%base_reg + STACK_FRAME_SZ + TR_G6], %g6;

#define LOAD_TR_INS(base_reg) \
	ldd [%base_reg + STACK_FRAME_SZ + TR_I0], %i0; \
	ldd [%base_reg + STACK_FRAME_SZ + TR_I2], %i2; \
	ldd [%base_reg + STACK_FRAME_SZ + TR_I4], %i4; \
	ldd [%base_reg + STACK_FRAME_SZ + TR_I6], %i6;

#define LOAD_TR_YREG(base_reg, scratch) \
	ld [%base_reg + STACK_FRAME_SZ + TR_Y], %scratch; \
	wr %scratch, 0x0, %y;

#define LOAD_TR_PRIV(base_reg, t_psr, t_pc, t_npc) \
	ld [%base_reg + STACK_FRAME_SZ + TR_PSR], %t_psr; \
	ld [%base_reg + STACK_FRAME_SZ + TR_PC],  %t_pc;  \
	ld [%base_reg + STACK_FRAME_SZ + TR_NPC], %t_npc;

#define LOAD_TR_ALL(base_reg, t_psr, t_pc, t_npc, g_scratch) \
	LOAD_TR_UNPRIV(base_reg, g_scratch)       \
	LOAD_TR_PRIV(base_reg, t_psr, t_pc, t_npc)

#define LOAD_TR_UNPRIV(base_reg, g_scratch) \
	LOAD_TR_YREG(base_reg, g_scratch) \
	LOAD_TR_GLOBALS(base_reg)         \
	LOAD_TR_INS(base_reg)

#define STORE_TR_GLOBALS(base_reg) \
	st  %g1, [%base_reg + STACK_FRAME_SZ + TR_G1]; \
	std %g2, [%base_reg + STACK_FRAME_SZ + TR_G2]; \
	std %g4, [%base_reg + STACK_FRAME_SZ + TR_G4]; \
	std %g6, [%base_reg + STACK_FRAME_SZ + TR_G6];

#define STORE_TR_INS(base_reg) \
	std %i0, [%base_reg + STACK_FRAME_SZ + TR_I0]; \
	std %i2, [%base_reg + STACK_FRAME_SZ + TR_I2]; \
	std %i4, [%base_reg + STACK_FRAME_SZ + TR_I4]; \
	std %i6, [%base_reg + STACK_FRAME_SZ + TR_I6];

#define STORE_TR_YREG(base_reg, scratch) \
	rd %y, %scratch;                                \
	st %scratch, [%base_reg + STACK_FRAME_SZ + TR_Y];

#define STORE_TR_UNPRIV(base_reg, g_scratch) \
	STORE_TR_INS(base_reg)             \
	STORE_TR_GLOBALS(base_reg)         \
	STORE_TR_YREG(base_reg, g_scratch)

#define STORE_TR_PRIV(base_reg, t_psr, t_pc, t_npc) \
	st %t_psr, [%base_reg + STACK_FRAME_SZ + TR_PSR]; \
	st %t_pc,  [%base_reg + STACK_FRAME_SZ + TR_PC];  \
	st %t_npc, [%base_reg + STACK_FRAME_SZ + TR_NPC];

#define STORE_TR_ALL(base_reg, t_psr, t_pc, t_npc, g_scratch) \
	STORE_TR_PRIV(base_reg, t_psr, t_pc, t_npc) \
    STORE_TR_UNPRIV(base_reg, g_scratch)

#endif /* __ASSEMBLER__ */

#endif /* SPARC_TRAP_REGS_H_ */

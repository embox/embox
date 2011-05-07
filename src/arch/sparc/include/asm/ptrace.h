/**
 * @file
 * @brief sparc specific parts of process tracing
 *
 * @date 23.07.09
 * @author Eldar Abusalimov
 */

#ifndef SPARC_PTRACE_H_
#define SPARC_PTRACE_H_

#include <types.h>
#include <asm/stack.h>

#ifndef __ASSEMBLER__

#define PTRACE_REGS_SZ sizeof(struct pt_regs)

/**
 * Basic set of registers to save when trap occurs.
 */
struct pt_regs {
	uint32_t psr;        /**< Processor state register. */
	uint32_t pc;         /**< Program counter. */
	uint32_t npc;        /**< Next Program counter. */
	uint32_t y;          /**< MUL/DIV flags. */
	uint32_t globals[8]; /**< Global registers. */
	uint32_t ins[8];     /**< Input registers. */
}__attribute__ ((aligned (8)));

#else /* __ASSEMBLER__ */

/* compute sizes by hand (see above) */
#define PTRACE_REGS_SZ ((1+1+1+1+8+8)*4)

/* Offsets for trap context structure  */
#define PT_PSR    0x0
#define PT_PC     0x4
#define PT_NPC    0x8
#define PT_Y      0xc
#define PT_G0     0x10
#define PT_G1     0x14
#define PT_G2     0x18
#define PT_G3     0x1c
#define PT_G4     0x20
#define PT_G5     0x24
#define PT_G6     0x28
#define PT_G7     0x2c
#define PT_I0     0x30
#define PT_I1     0x34
#define PT_I2     0x38
#define PT_I3     0x3c
#define PT_I4     0x40
#define PT_I5     0x44
#define PT_I6     0x48
#define PT_I7     0x4c

#define LOAD_PT_GLOBALS(base_reg) \
	ld  [%base_reg + STACK_FRAME_SZ + PT_G1], %g1; \
	ldd [%base_reg + STACK_FRAME_SZ + PT_G2], %g2; \
	ldd [%base_reg + STACK_FRAME_SZ + PT_G4], %g4; \
	ldd [%base_reg + STACK_FRAME_SZ + PT_G6], %g6;

#define LOAD_PT_INS(base_reg) \
	ldd [%base_reg + STACK_FRAME_SZ + PT_I0], %i0; \
	ldd [%base_reg + STACK_FRAME_SZ + PT_I2], %i2; \
	ldd [%base_reg + STACK_FRAME_SZ + PT_I4], %i4; \
	ldd [%base_reg + STACK_FRAME_SZ + PT_I6], %i6;

#define LOAD_PT_YREG(base_reg, scratch) \
	ld [%base_reg + STACK_FRAME_SZ + PT_Y], %scratch; \
	wr %scratch, 0x0, %y;

#define LOAD_PT_PRIV(base_reg, t_psr, t_pc, t_npc) \
	ld [%base_reg + STACK_FRAME_SZ + PT_PSR], %t_psr; \
	ld [%base_reg + STACK_FRAME_SZ + PT_PC],  %t_pc;  \
	ld [%base_reg + STACK_FRAME_SZ + PT_NPC], %t_npc;

#define LOAD_PT_ALL(base_reg, t_psr, t_pc, t_npc, g_scratch) \
	LOAD_PT_UNPRIV(base_reg, g_scratch)       \
	LOAD_PT_PRIV(base_reg, t_psr, t_pc, t_npc)

#define LOAD_PT_UNPRIV(base_reg, g_scratch) \
	LOAD_PT_YREG(base_reg, g_scratch) \
	LOAD_PT_GLOBALS(base_reg)         \
	LOAD_PT_INS(base_reg)

#define STORE_PT_GLOBALS(base_reg) \
	st  %g1, [%base_reg + STACK_FRAME_SZ + PT_G1]; \
	std %g2, [%base_reg + STACK_FRAME_SZ + PT_G2]; \
	std %g4, [%base_reg + STACK_FRAME_SZ + PT_G4]; \
	std %g6, [%base_reg + STACK_FRAME_SZ + PT_G6];

#define STORE_PT_INS(base_reg) \
	std %i0, [%base_reg + STACK_FRAME_SZ + PT_I0]; \
	std %i2, [%base_reg + STACK_FRAME_SZ + PT_I2]; \
	std %i4, [%base_reg + STACK_FRAME_SZ + PT_I4]; \
	std %i6, [%base_reg + STACK_FRAME_SZ + PT_I6];

#define STORE_PT_YREG(base_reg, scratch) \
	rd %y, %scratch;                                \
	st %scratch, [%base_reg + STACK_FRAME_SZ + PT_Y];

#define STORE_PT_UNPRIV(base_reg, g_scratch) \
	STORE_PT_INS(base_reg)             \
	STORE_PT_GLOBALS(base_reg)         \
	STORE_PT_YREG(base_reg, g_scratch)

#define STORE_PT_PRIV(base_reg, t_psr, t_pc, t_npc) \
	st %t_psr, [%base_reg + STACK_FRAME_SZ + PT_PSR]; \
	st %t_pc,  [%base_reg + STACK_FRAME_SZ + PT_PC];  \
	st %t_npc, [%base_reg + STACK_FRAME_SZ + PT_NPC];

#define STORE_PT_ALL(base_reg, t_psr, t_pc, t_npc, g_scratch) \
	STORE_PT_PRIV(base_reg, t_psr, t_pc, t_npc) \
	STORE_PT_UNPRIV(base_reg, g_scratch)

#endif /* __ASSEMBLER__ */

#endif /* SPARC_PTRACE_H_ */

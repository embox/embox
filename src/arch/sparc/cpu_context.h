/**
 * \file cpu_context.h
 *
 * \brief Defines CPU context structure and offsets
 *	TODO comments = shit
 * \date Jan 26, 2009
 * \author Eldar Abusalimov
 */

#ifndef CPU_CONTEXT_H_
#define CPU_CONTEXT_H_

#include "leon_config.h"
#include "traps.h"
#include "types.h"
#include "windows.h"

#define WINDOWS_QUANTITY		0x8
#define GLOBAL_REGS_QUANTITY	0x8
#define LOCAL_REGS_QUANTITY		0x10

#define CPU_CONTEXT_WIM			0x00
#define CPU_CONTEXT_PSR			0x04
#define CPU_CONTEXT_Y			0x08
#define CPU_CONTEXT_TBR			0x0C
#define CPU_CONTEXT_SP			0x10
#define CPU_CONTEXT_FP			0x14
#define CPU_CONTEXT_GLOB		0x18
#define CPU_CONTEXT_LOCAL		CPU_CONTEXT_GLOB + (8 * 4)
#define CPU_CONTEXT_PC			CPU_CONTEXT_LOCAL + (WINDOWS_QUANTITY * LOCAL_REGS_QUANTITY * 4)
#define CPU_CONTEXT_NPC			CPU_CONTEXT_PC + 4

#ifdef __ASSEMBLER__

/* base address to save (restore) context to (from) */
#define l_base  l6
/* return address */
#define l_retpc l7

/* l0 for t_psr compatibility */
#define l_psr   t_psr
/* l1 for t_pc compatibility */
#define l_pc    t_pc
/* l2 for t_npc compatibility */
#define l_npc   t_npc

#endif /* __ASSEMBLER__ */

#ifndef __ASSEMBLER__

/*
 * Structure holding the whole context of the CPU
 */
typedef  struct _CPU_CONTEXT {
	/* CWP field of PSR register */
	WORD psr_cwp;
	/* global registers */
	WORD globals[7];
	/* all register windows */
	REG_WINDOW reg_windows[CORE_NWINDOWS];
/*
 * We do not explicitly define space
 * for PSR, PC, nPC, Y, WIM and TBR registers.
 * These registers are saved in locals of the window
 * pointed at the call time
 */
} __attribute__ ((aligned (8))) CPU_CONTEXT;

#define CPU_CONTEXT_SZ sizeof(CPU_CONTEXT)

#else /* __ASSEMBLER__ */

/* compute sizes by hand */
#define CPU_CONTEXT_SZ (4 * 8 + REG_WINDOW_SZ * CORE_NWINDOWS)

/* Offsets for trap context structure  */
#define CC_PSR_CWP 0x00
#define CC_G1      0x04
#define CC_G2      0x08
#define CC_G3      0x0c
#define CC_G4      0x10
#define CC_G5      0x14
#define CC_G6      0x18
#define CC_G7      0x1c
#define CC_REG_WINDOWS 0x20

#define LOAD_CC_PSR_CWP(base_reg,  cwp_reg) \
	ld      [%base_reg + CC_PSR_CWP], %cwp_reg; \

#define LOAD_CC_GLOBALS(base_reg) \
	ld      [%base_reg + CC_G1], %g1; \
	ldd     [%base_reg + CC_G2], %g2; \
	ldd     [%base_reg + CC_G4], %g4; \
	ldd     [%base_reg + CC_G6], %g6;

#define LOAD_CC_REG_WINDOW(base_reg) \
	LOAD_WINDOW(base_reg, CC_REG_WINDOWS)

#define STORE_CC_PSR_CWP(base_reg, cwp_reg) \
	st      %cwp_reg, [%base_reg + CC_PSR_CWP]; \

#define STORE_CC_GLOBALS(base_reg) \
	st      %g1, [%base_reg + CC_G1]; \
	std     %g2, [%base_reg + CC_G2]; \
	std     %g4, [%base_reg + CC_G4]; \
	std     %g6, [%base_reg + CC_G6];

#define STORE_CC_REG_WINDOW(base_reg) \
	STORE_WINDOW(base_reg, CC_REG_WINDOWS);

#endif /* __ASSEMBLER__ */

#endif /* CPU_CONTEXT_H_ */

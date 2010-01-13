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

#ifdef _TEST_SYSTEM_
#include <asm/leon_config.h>
#include <types.h>
#endif
#include <asm/traps.h>
#include <asm/winmacro.h>

#ifndef __ASSEMBLER__

/*
 * Structure holding the whole context of the CPU
 */
typedef struct _CPU_CONTEXT {
	/* global registers */
	uint32_t globals[8];
	/* some control registers */
	uint32_t y, tbr, psr, wim;
	/* all register windows */
	REG_WINDOW reg_windows[CORE_NWINDOWS];
/*
 * We do not explicitly define space
 * for PC and nPC registers.
 * These registers are saved in locals of the window
 * pointed at the call time. (see traps.h)
 */
}__attribute__ ((aligned (8))) CPU_CONTEXT;

#else /* __ASSEMBLER__ */

/* base address to save (restore) context to (from) */
#define l_base  t2
/* return address */
#define l_retpc t_retpc

/* Offsets for trap context structure  */
#define CC_G0      0x00
#define CC_G1      0x04
#define CC_G2      0x08
#define CC_G3      0x0c
#define CC_G4      0x10
#define CC_G5      0x14
#define CC_G6      0x18
#define CC_G7      0x1c
#define CC_YREG    0x20
#define CC_TBR     0x24
#define CC_PSR     0x28
#define CC_WIM     0x2c
#define CC_REG_WINDOWS 0x30

#define LOAD_CC_PRIV(base_reg, scratch) \
	ld      [%base_reg + CC_YREG], %scratch;\
	wr      %scratch, %g0, %y;              \
	ld      [%base_reg + CC_TBR], %scratch; \
	wr      %scratch, %g0, %tbr;            \
	ld      [%base_reg + CC_WIM], %scratch; \
	wr      %scratch, %g0, %wim;            \
	ld      [%base_reg + CC_PSR], %scratch; \
	wr      %scratch, %g0, %psr;            \
	 WRITE_PAUSE;

#define LOAD_CC_GLOBALS(base_reg) \
	ld      [%base_reg + CC_G1], %g1; \
	ldd     [%base_reg + CC_G2], %g2; \
	ldd     [%base_reg + CC_G4], %g4; \
	ldd     [%base_reg + CC_G6], %g6;

#define LOAD_CC_REG_WINDOW(base_reg) \
	LOAD_WINDOW(base_reg, CC_REG_WINDOWS);

#define STORE_CC_PRIV(base_reg, scratch) \
	rd      %psr, %scratch;                 \
	st      %scratch, [%base_reg + CC_PSR]; \
	rd      %y, %scratch;                   \
	st      %scratch, [%base_reg + CC_YREG];\
	rd      %tbr, %scratch;                 \
	st      %scratch, [%base_reg + CC_TBR]; \
	rd      %wim, %scratch;                 \
	st      %scratch, [%base_reg + CC_WIM];

#define STORE_CC_GLOBALS(base_reg) \
	st      %g1, [%base_reg + CC_G1]; \
	std     %g2, [%base_reg + CC_G2]; \
	std     %g4, [%base_reg + CC_G4]; \
	std     %g6, [%base_reg + CC_G6];

#define STORE_CC_REG_WINDOW(base_reg) \
	STORE_WINDOW(base_reg, CC_REG_WINDOWS);

#endif /* __ASSEMBLER__ */

#endif /* CPU_CONTEXT_H_ */

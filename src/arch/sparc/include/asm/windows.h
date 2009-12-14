/**
 * \file windows.h
 *
 * \brief SPARC register windows related macros
 *
 * \date Jun 4, 2009
 * \author Eldar Abusalimov
 */

#ifndef WINDOWS_H_
#define WINDOWS_H_

#include "types.h"
#include "asm/leon_config.h"

#ifndef __ASSEMBLER__

/**
 * A register window
 */
typedef struct _REG_WINDOW {
	uint32_t locals[8];
	uint32_t ins[8];
} REG_WINDOW;

#define REG_WINDOW_SZ sizeof(REG_WINDOW)

#else /* __ASSEMBLER__ */

/** compute sizes by hand */
#define REG_WINDOW_SZ ((8+8)*4)

/**
 * REG_WINDOW offsets
 */
#define RW_L0     0x00
#define RW_L1     0x04
#define RW_L2     0x08
#define RW_L3     0x0c
#define RW_L4     0x10
#define RW_L5     0x14
#define RW_L6     0x18
#define RW_L7     0x1c
#define RW_I0     0x20
#define RW_I1     0x24
#define RW_I2     0x28
#define RW_I3     0x2c
#define RW_I4     0x30
#define RW_I5     0x34
#define RW_I6     0x38
#define RW_I7     0x3c

/**
 * Store the register window onto the 8-byte aligned area starting at %reg.
 */
#define STORE_WINDOW(reg, offset) \
	std	%l0, [%reg + (offset) + RW_L0]; \
	std	%l2, [%reg + (offset) + RW_L2]; \
	std	%l4, [%reg + (offset) + RW_L4]; \
	std	%l6, [%reg + (offset) + RW_L6]; \
	std	%i0, [%reg + (offset) + RW_I0]; \
	std	%i2, [%reg + (offset) + RW_I2]; \
	std	%i4, [%reg + (offset) + RW_I4]; \
	std	%i6, [%reg + (offset) + RW_I6];

/**
 * Load a register window from the 8-byte aligned area beginning at %reg.
 */
#define LOAD_WINDOW(reg, offset) \
	ldd	[%reg + (offset) + RW_L0], %l0; \
	ldd	[%reg + (offset) + RW_L2], %l2; \
	ldd	[%reg + (offset) + RW_L4], %l4; \
	ldd	[%reg + (offset) + RW_L6], %l6; \
	ldd	[%reg + (offset) + RW_I0], %i0; \
	ldd	[%reg + (offset) + RW_I2], %i2; \
	ldd	[%reg + (offset) + RW_I4], %i4; \
	ldd	[%reg + (offset) + RW_I6], %i6;

#endif /* __ASSEMBLER__ */

#endif /* WINDOWS_H_ */

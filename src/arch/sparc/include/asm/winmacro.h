/**
 * @file winmacro.h
 *
 * @brief SPARC register windows related macros
 *
 * @date Jun 4, 2009
 * @author Eldar Abusalimov
 */

#ifndef SPARC_WINDOWS_H_
#define SPARC_WINDOWS_H_

#include <asm/ptrace.h>

#ifdef __ASSEMBLER__

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

#endif /* SPARC_WINDOWS_H_ */

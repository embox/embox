/**
 * @file
 * @brief SPARC register windows related stuff.
 * @details Here is also described the windows management policy
 * used in the core trap setup and WOF/WUF handling routines.
 *
 * We do a small trick based on a nonstandard usage of the WIM register.
 * The main advantage of this method is that it allows to quickly determine
 * a presence of user windows on the CPU.
 *
 * More definitely, we encode in the WIM not only the invalid window marker
 * but also the mask of the last user window (if any) being on the CPU.
 * Thus, one may distinguish WIM bits to @b primary (stands for traditional
 * invalid window mask) and @b secondary (represents the last user window)
 * markers.
 *
 * Some restrictions and assumptions needed for this method to work properly:
 *   @n 1) Kernel code does not "over-restores" (particularly, it should
 * not perform "RESTORE - SAVE" combinations).
 *   @n 2) At any moment each kernel window has proper stack pointer.
 *   @n 3) CPU has got 3 or more register windows. Because of this fact it is
 * guaranteed that bitwise AND of next and previous windows masks is always 0.
 * This means that ORing, XORing, ADDing these masks together give the same
 * result, and we can join such operations with SAVE or WRWIM instructions.
 *   TODO
 *
 * @date Jun 4, 2009
 * @author Eldar Abusalimov
 */

#ifndef SPARC_WINDOWS_H_
#define SPARC_WINDOWS_H_

#include <types.h>

#ifndef __ASSEMBLER__

#define REG_WINDOW_SZ sizeof(struct reg_window)

/**
 * SPARC register window.
 */
struct reg_window {
	uint32_t locals[8];
	uint32_t ins[8];
}__attribute__ ((aligned (8)));

extern void winflush(void);

#else /* __ASSEMBLER__ */

/* compute sizes by hand */
#define REG_WINDOW_SZ ((8+8)*4)

/* reg_window offsets */
#define RW_L0 0x00
#define RW_L1 0x04
#define RW_L2 0x08
#define RW_L3 0x0c
#define RW_L4 0x10
#define RW_L5 0x14
#define RW_L6 0x18
#define RW_L7 0x1c
#define RW_I0 0x20
#define RW_I1 0x24
#define RW_I2 0x28
#define RW_I3 0x2c
#define RW_I4 0x30
#define RW_I5 0x34
#define RW_I6 0x38
#define RW_I7 0x3c

/**
 * Store the register window onto the 8-byte aligned area starting at %reg.
 */
#define STORE_WINDOW(reg) \
	std %l0, [%reg + RW_L0]; \
	std %l2, [%reg + RW_L2]; \
	std %l4, [%reg + RW_L4]; \
	std %l6, [%reg + RW_L6]; \
	std %i0, [%reg + RW_I0]; \
	std %i2, [%reg + RW_I2]; \
	std %i4, [%reg + RW_I4]; \
	std %i6, [%reg + RW_I6];

/**
 * Load a register window from the 8-byte aligned area beginning at %reg.
 */
#define LOAD_WINDOW(reg) \
	ldd [%reg + RW_L0], %l0; \
	ldd [%reg + RW_L2], %l2; \
	ldd [%reg + RW_L4], %l4; \
	ldd [%reg + RW_L6], %l6; \
	ldd [%reg + RW_I0], %i0; \
	ldd [%reg + RW_I2], %i2; \
	ldd [%reg + RW_I4], %i4; \
	ldd [%reg + RW_I6], %i6;

#ifdef CONFIG_SRMMU

/**
 * Performs basic stack pointer checks and probes the MMU
 * for the mappings that correspond to the %sp value.
 */
#define USER_STACK_PROBE(corrupt_stack_branch, scratch) \
	/* Check stack alignment. */               \
	andcc %sp, 0x7, %g0;                       \
	bne corrupt_stack_branch;                  \
	 /* Check if %sp and %sp + 0x38            \
	  * point to the different pages. */       \
	 add %sp, REG_WINDOW_SZ - 8, %scratch;     \
	xor %scratch, %sp, %scratch;               \
	andcc %scratch, 0x1000, %g0;               \
	andn %sp, 0xfff, %scratch;                 \
	be 10293847f; /* single page */            \
	 or %scratch, 0x400, %scratch;             \
	/* Probability of reaching the following   \
	 * 6 instructions is 1/64. */              \
	add %scratch, 0x1000, %scratch;            \
	/* Probe the MMU for %sp + 0x38 value. */  \
	lda [%scratch] ASI_LEON_MMUFLUSH, %scratch;\
	andcc %scratch, MMU_PTE_WRITE, %g0;          \
	be corrupt_stack_branch;                   \
	 andn %sp, 0xfff, %scratch;                \
	or %scratch, 0x400, %scratch;              \
10293847: /* single page */                    \
	 /* Probe the MMU for %sp value. */        \
	lda [%scratch] ASI_LEON_MMUFLUSH, %scratch;\
	andcc %scratch, MMU_PTE_WRITE, %g0;          \
	be corrupt_stack_branch;                   \
	 nop;

#else
#define USER_STACK_PROBE(corrupt_stack_branch, scratch)
#endif /* CONFIG_SRMMU */

#define STORE_USER_WINDOW(reg, corrupt_stack_branch, scratch) \
	USER_STACK_PROBE(corrupt_stack_branch, scratch) \
	STORE_WINDOW(reg)

#define LOAD_USER_WINDOW(reg, corrupt_stack_branch, scratch) \
	USER_STACK_PROBE(corrupt_stack_branch, scratch) \
	LOAD_WINDOW(reg)

#endif /* __ASSEMBLER__ */

#endif /* SPARC_WINDOWS_H_ */

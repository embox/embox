/**
 * @file
 * @brief Defines stack frame structure.
 *
 * @date 29.11.09
 * @author Eldar Abusalimov
 */

#ifndef SPARC_STACK_H_
#define SPARC_STACK_H_

#include <types.h>

#include <asm/winmacro.h>

/** SPARC stack is doubleword aligned. */
#define STACK_ALIGNMENT 8

#ifndef __ASSEMBLER__

/** The size of minimal required stack frame. */
#define STACK_FRAME_SZ  sizeof(struct stack_frame)

/**
 * Stack frame structure accordingly to the SPARC v8 Software Considerations.
 */
struct stack_frame {
	/** 16 words in which to save register window (in and local registers). */
	struct reg_window reg_window;
	/** One-word hidden parameter (address for aggregate return value). */
	uint32_t structptr[1];
	/** 6 words into which callee may store register arguments. */
	uint32_t xargs[6];
	/** TODO I don't know. Seems to be just a place holder. -- Eldar */
	uint32_t xxargs[1];
}__attribute__ ((aligned (STACK_ALIGNMENT)));

#else /* __ASSEMBLER__ */

/* compute sizes by hand (see above) */
#define STACK_FRAME_SZ (REG_WINDOW_SZ + (1+6+1)*4)

#endif /* __ASSEMBLER__ */

#endif /* SPARC_STACK_H_ */

/**
 * @file
 *
 * @date Jun 4, 2009
 * @author Eldar Abusalimov
 */

#ifndef SPARC_PTRACE_H_
#define SPARC_PTRACE_H_

#include <types.h>
#include <asm/leon_config.h>

#ifndef __ASSEMBLER__

/**
 * A register window
 */
struct reg_window {
	uint32_t locals[8];
	uint32_t ins[8];
};

#define REG_WINDOW_SZ sizeof(struct reg_window)

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

#endif /* __ASSEMBLER__ */

#endif /* SPARC_PTRACE_H_ */

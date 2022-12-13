/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.05.2014
 */

#ifndef ARM__PTRACE_H_
#define ARM__PTRACE_H_

#define PTREGS_SIZE (17 * 4)

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef struct pt_regs {
	uint32_t r[13];
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t cpsr;
} pt_regs_t;

static inline void ptregs_retcode(struct pt_regs *ptregs, int retcode) {
	ptregs->r[0] = retcode;
}

#else /* __ASSEMBLER__ */

#define CALL_WPTREGS \
	sub     sp, sp, #68;  \
	stmia   sp, {r0 - r12, r14}; \
	str     sp, [sp, #56]; \
	mov     r0, sp;       \
	bl


#endif /* __ASSEMBLER__ */

#endif /* ARM__PTRACE_H_ */

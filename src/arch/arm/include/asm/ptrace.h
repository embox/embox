/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    15.05.2014
 */

#ifndef ARM__PTRACE_H_
#define ARM__PTRACE_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef struct pt_regs {
	int r[16];
	int psr;
} pt_regs_t;

static inline void ptregs_retcode(struct pt_regs *ptregs, int retcode) {
	ptregs->r[0] = retcode;
}

#endif /* __ASSEMBLER__ */

#endif /* ARM__PTRACE_H_ */

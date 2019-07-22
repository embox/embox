/**
 * @file ptrace.h
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 19.07.2019
 */

#ifndef AARCH64_PTRACE_H_
#define AARCH64_PTRACE_H_

#ifndef __ASSEMBLER__

#include <stdint.h>

typedef struct pt_regs {
	uint64_t x[31];
} pt_regs_t;

static inline void ptregs_retcode(struct pt_regs *ptregs, int retcode) {
	ptregs->x[0] = retcode;
}

#endif /* __ASSEMBLER__ */

#endif /* AARCH64_PTRACE_H_ */

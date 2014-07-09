/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

#ifndef HAL_PTRACE_H_
#define HAL_PTRACE_H_

#include <asm/ptrace.h>
#include <kernel/panic.h>

struct pt_regs;

extern void ptregs_retcode(struct pt_regs *ptregs, int retcode);
extern void __attribute__((noreturn)) ptregs_jmp(struct pt_regs *ptregs);

static inline void ptregs_retcode_jmp(struct pt_regs *ptregs, int retcode) {
	if (retcode < 0) {
		SET_ERRNO(-retcode);
		//return;
	}

	ptregs_retcode(ptregs, retcode);
	ptregs_jmp(ptregs);

	panic("ptregs_retcode_jmp returning");
}

#endif /* HAL_PTRACE_H_ */

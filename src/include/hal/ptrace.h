/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    02.06.2014
 */

#ifndef HAL_PTRACE_H_
#define HAL_PTRACE_H_

#include <errno.h>
#include <asm/ptrace.h>
#include <kernel/panic.h>
#include <compiler.h>

struct pt_regs;

#define ptregs_retcode_jmp(ptregs, retcode) ptregs_retcode_err_jmp(ptregs, retcode, ENOERR)

extern void ptregs_retcode(struct pt_regs *ptregs, int retcode);
extern void _NORETURN ptregs_jmp(struct pt_regs *ptregs);

static inline void ptregs_retcode_err_jmp(struct pt_regs *ptregs, int retcode, int errnum) {
	if (errnum != ENOERR) {
		SET_ERRNO(errnum);
	}

	ptregs_retcode(ptregs, retcode);
	ptregs_jmp(ptregs);

	panic("ptregs_retcode_jmp returning");
}

#endif /* HAL_PTRACE_H_ */

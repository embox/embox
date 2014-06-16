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

struct pt_regs;

extern void ptregs_retcode(struct pt_regs *ptregs, int retcode);

extern void __attribute__((noreturn)) ptregs_jmp(struct pt_regs *ptregs);

#endif /* HAL_PTRACE_H_ */

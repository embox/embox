/**
 * @file
 *
 * @date Jul 12, 2014
 * @author Anton Bondarev
 */

#ifndef VFORK_RES_STOP_PARENT_H_
#define VFORK_RES_STOP_PARENT_H_

#include <sys/types.h>
#include <asm/ptrace.h>
#include <setjmp.h>

#define VFORK_CTX_STACK_LEN 0x1000

struct task_vfork {
	struct pt_regs ptregs;
	pid_t child_pid;
	jmp_buf env;
	void *stack;
};

#endif /* VFORK_RES_STOP_PARENT_H_ */

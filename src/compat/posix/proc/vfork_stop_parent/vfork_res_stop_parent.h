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

#if 0
#include <module/embox/arch/context.h>

#define VFORK_CTX_STACK_LEN 0x1000
struct vfork_ctx {
	struct pt_regs ptregs;
	struct context original_ctx;
	struct context waiting_ctx;
	char stack[VFORK_CTX_STACK_LEN] __attribute__((aligned(4)));
	bool parent_blocked;
	int child_pid;
};
#endif

struct task_vfork {
	struct pt_regs ptregs;
	pid_t child_pid;
	int parent_blocked;
	jmp_buf env;
	char (*stack) [VFORK_CTX_STACK_LEN]; // __attribute__((aligned(4)));
};

#endif /* VFORK_RES_STOP_PARENT_H_ */

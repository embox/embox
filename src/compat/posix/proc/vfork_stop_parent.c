/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.05.2014
 */

#include <mem/sysmalloc.h>
#include <asm/traps.h>
#include <kernel/panic.h>
#include <kernel/sched.h>
#include <kernel/task.h>

extern void __attribute__((noreturn)) vfork_leave(struct pt_regs *ptregs);

#define VFORK_CTX_STACK_LEN 0x1000
struct vfork_ctx {
	struct pt_regs ptregs;
	struct context original_ctx;
	struct context waiting_ctx;
	bool parent_holded;
	char stack[VFORK_CTX_STACK_LEN];
};

static struct vfork_ctx *vfork_current_context;

static void ptregs_retcode(struct pt_regs *ptregs, int retcode) {
	ptregs->eax = retcode;
}

static void vfork_parent_signal_handler(int sig, siginfo_t *siginfo, void *context) {
	struct vfork_ctx *vfctx = vfork_current_context;

	vfctx->parent_holded = false;
}

static void *vfork_child_task(void *arg) {
	struct pt_regs *ptregs = arg;

	ptregs_retcode(ptregs, 0);
	vfork_leave(ptregs);

	panic("vfork_child_task returning");
}

static void vfork_waiting(void) {
	struct sigaction sa, ochildsa, ocontsa;
	struct vfork_ctx *vfctx;
	pid_t child;

	vfctx = vfork_current_context;

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = vfork_parent_signal_handler;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGCHLD, &sa, &ochildsa);
	sigaction(SIGCONT, &sa, &ocontsa);

	vfctx->parent_holded = true;

	child = new_task("", vfork_child_task, &vfctx->ptregs);

	SCHED_WAIT(!vfctx->parent_holded);

	sigaction(SIGCHLD, &ochildsa, NULL);
	sigaction(SIGCONT, &ocontsa, NULL);

	ptregs_retcode(&vfctx->ptregs, child);
	context_switch(&vfctx->waiting_ctx, &vfctx->original_ctx);

	panic("vfork_waiting returning");
}

void __attribute__((noreturn)) vfork_body(struct pt_regs *ptregs) {
	struct pt_regs ptbuf;
	struct vfork_ctx *vfctx;

	vfctx = sysmalloc(sizeof(*vfctx));
	if (!vfctx) {
		ptregs_retcode(ptregs, -EAGAIN);
		vfork_leave(&ptbuf);
	}

	memcpy(&vfctx->ptregs, ptregs, sizeof(vfctx->ptregs));

	context_init(&vfctx->waiting_ctx, true);
	context_set_entry(&vfctx->waiting_ctx, vfork_waiting);
	context_set_stack(&vfctx->waiting_ctx, vfctx->stack + sizeof(vfctx->stack));

	vfork_current_context = vfctx;
	context_switch(&vfctx->original_ctx, &vfctx->waiting_ctx);

	/* current stack is broken, can't reach any old data */

	vfctx = vfork_current_context;
	memcpy(&ptbuf, &vfctx->ptregs, sizeof(*ptregs));
	sysfree(vfctx);

	vfork_leave(&ptbuf);

	panic("vfork_body returning");
}

void vfork_release_parent(void) {
	kill(task_get_id(task_get_parent(task_self())), SIGCONT);
}


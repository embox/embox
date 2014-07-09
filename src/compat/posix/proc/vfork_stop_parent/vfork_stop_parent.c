/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.05.2014
 */

#include <mem/sysmalloc.h>
#include <hal/ptrace.h>
#include <hal/vfork.h>
#include <kernel/panic.h>
#include <kernel/sched.h>
#include <kernel/task.h>

#define VFORK_CTX_STACK_LEN 0x1000
struct vfork_ctx {
	struct pt_regs ptregs;
	struct context original_ctx;
	struct context waiting_ctx;
	char stack[VFORK_CTX_STACK_LEN] __attribute__((aligned(4)));
	bool parent_holded;
	int child_pid;
};

static struct vfork_ctx *vfork_current_context;

static void vfork_parent_signal_handler(int sig, siginfo_t *siginfo, void *context) {
	struct vfork_ctx *vfctx = vfork_current_context;

	vfctx->parent_holded = false;
}

static void *vfork_child_task(void *arg) {
	struct pt_regs *ptregs = arg;

	ptregs_retcode(ptregs, 0);
	ptregs_jmp(ptregs);

	panic("vfork_child_task returning");
}

static void vfork_wait_signal_store(struct sigaction *ochildsa,
	       	struct sigaction *ocontsa) {
	struct sigaction sa;

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = vfork_parent_signal_handler;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGCHLD, &sa, ochildsa);
	sigaction(SIGCONT, &sa, ocontsa);
}

static void vfork_wait_signal_restore(const struct sigaction *ochildsa,
	       	const struct sigaction *ocontsa) {
	sigaction(SIGCHLD, ochildsa, NULL);
	sigaction(SIGCONT, ocontsa, NULL);
}

static void vfork_waiting(void) {
	struct sigaction ochildsa, ocontsa;
	struct vfork_ctx *vfctx;
	struct task *child;

	vfctx = vfork_current_context;

	vfork_wait_signal_store(&ochildsa, &ocontsa);
	{
		vfctx->parent_holded = true;

		child = task_table_get(vfctx->child_pid);
		task_start(child, vfork_child_task, &vfctx->ptregs);

		SCHED_WAIT(!vfctx->parent_holded);
	}
	vfork_wait_signal_restore(&ochildsa, &ocontsa);

	ptregs_retcode(&vfctx->ptregs, vfctx->child_pid);
	context_switch(&vfctx->waiting_ctx, &vfctx->original_ctx);

	panic("vfork_waiting returning");
}

void __attribute__((noreturn)) vfork_body(struct pt_regs *ptregs) {
	struct pt_regs ptbuf;
	struct vfork_ctx *vfctx;
	pid_t child_pid;

	child_pid = task_prepare("");
	if (0 > child_pid) {
		/* error */
		SET_ERRNO(child_pid);
		ptregs_retcode(ptregs, -1);
		ptregs_jmp(ptregs);
		panic("vfork_body returning");
	}

	vfctx = sysmalloc(sizeof(*vfctx));
	if (!vfctx) {
		ptregs_retcode(ptregs, -EAGAIN);
		ptregs_jmp(&ptbuf);
	}

	memcpy(&vfctx->ptregs, ptregs, sizeof(vfctx->ptregs));
	vfctx->child_pid = child_pid;

	context_init(&vfctx->waiting_ctx, true);
	context_set_entry(&vfctx->waiting_ctx, vfork_waiting);
	context_set_stack(&vfctx->waiting_ctx, vfctx->stack + sizeof(vfctx->stack));

	vfork_current_context = vfctx;
	context_switch(&vfctx->original_ctx, &vfctx->waiting_ctx);

	/* current stack is broken, can't reach any old data */

	vfctx = vfork_current_context;
	memcpy(&ptbuf, &vfctx->ptregs, sizeof(*ptregs));
	sysfree(vfctx);

	ptregs_jmp(&ptbuf);

	panic("vfork_body returning");
}

void vfork_release_parent(void) {
	kill(task_get_id(task_get_parent(task_self())), SIGCONT);
}


//FIXME
void vfork_child_done(struct task *child, void * (*run)(void *)) {

}


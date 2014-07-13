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
#include <kernel/task/resource/task_vfork.h>

#define VFORK_CTX_STACK_LEN 0x1000
struct vfork_ctx {
//	struct pt_regs ptregs;
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

	ptregs_retcode_jmp(ptregs, 0);

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
	struct task_vfork *task_vfork;

	task_vfork = task_resource_vfork(task_self());
	vfctx = task_vfork->vfork_ctx;
	child = task_table_get(vfctx->child_pid);

	vfork_wait_signal_store(&ochildsa, &ocontsa);
	{
		vfctx->parent_holded = true;
		task_start(child, vfork_child_task, task_vfork->vfork_ctx);

		SCHED_WAIT(!vfctx->parent_holded);
	}
	vfork_wait_signal_restore(&ochildsa, &ocontsa);

//	ptregs_retcode(&vfctx->ptregs, vfctx->child_pid);
	context_switch(&vfctx->waiting_ctx, &vfctx->original_ctx);

	panic("vfork_waiting returning");
}

int vfork_callback(struct task *child) {
	//struct pt_regs *ptregs;
	struct vfork_ctx *vfctx;
	struct task_vfork *task_vfork;

	vfctx = sysmalloc(sizeof(*vfctx));
	if (!vfctx) {
		return -EAGAIN;
		//ptregs_retcode_jmp(ptregs, -EAGAIN);
	}

	//memcpy(&vfctx->ptregs, ptregs, sizeof(vfctx->ptregs));
	task_vfork = task_resource_vfork(child->parent);
	task_vfork->vfork_ctx = vfctx;

	vfctx->child_pid = child->tsk_id;


	context_init(&vfctx->waiting_ctx, true);
	context_set_entry(&vfctx->waiting_ctx, vfork_waiting);
	context_set_stack(&vfctx->waiting_ctx, vfctx->stack + sizeof(vfctx->stack));

	//vfork_current_context = vfctx;
	context_switch(&vfctx->original_ctx, &vfctx->waiting_ctx);

	/* current stack is broken, can't reach any old data */

	//vfctx = vfork_current_context;
	//memcpy(&ptbuf, &vfctx->ptregs, sizeof(*ptregs));
	sysfree(vfctx);

	//ptregs_jmp(&ptbuf);

	panic("vfork_body returning");

	return 0;
}

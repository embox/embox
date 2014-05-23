/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.05.2014
 */

#include <mem/sysmalloc.h>
#include <asm/traps.h>
#include <hal/vfork.h>
#include <kernel/panic.h>
#include <kernel/sched.h>
#include <kernel/task.h>

#define VFORK_CTX_STACK_LEN 0x1000
struct vfork_ctx {
	struct pt_regs ptregs;
	struct context original_ctx;
	struct context waiting_ctx;
	bool parent_holded;
	char stack[VFORK_CTX_STACK_LEN];
	char *user_stack_copy;
};

static struct vfork_ctx *vfork_current_context;

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

static void vfork_user_stack_store(struct vfork_ctx *vfctx) {
	size_t st_size;
	void *st_copy;

	st_size = thread_stack_get_size(thread_self());
	st_copy = sysmalloc(st_size);
	assert(st_copy); /* allocation successed */

	memcpy(st_copy, thread_stack_get(thread_self()), st_size);

	vfctx->user_stack_copy = st_copy;
}

static void vfork_user_stack_restore(struct vfork_ctx *vfctx) {

	/* assuming user stack size couldn't change */
	memcpy(thread_stack_get(thread_self()), vfctx->user_stack_copy,
			thread_stack_get_size(thread_self()));

	sysfree(vfctx->user_stack_copy);
}

static void vfork_waiting(void) {
	struct sigaction ochildsa, ocontsa;
	struct vfork_ctx *vfctx;
	pid_t child;

	vfctx = vfork_current_context;

	vfork_user_stack_store(vfctx);
	vfork_wait_signal_store(&ochildsa, &ocontsa);

	vfctx->parent_holded = true;
	child = new_task("", vfork_child_task, &vfctx->ptregs);
	SCHED_WAIT(!vfctx->parent_holded);

	vfork_wait_signal_restore(&ochildsa, &ocontsa);
	vfork_user_stack_restore(vfctx);

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


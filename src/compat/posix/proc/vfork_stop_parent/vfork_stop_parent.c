/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    14.05.2014
 */
#include <setjmp.h>

#include <mem/sysmalloc.h>
#include <hal/ptrace.h>
#include <hal/vfork.h>
#include <kernel/panic.h>
#include <kernel/sched.h>
#include <kernel/task.h>
#include <kernel/task/resource/task_vfork.h>
#include <kernel/thread/thread_sched_wait.h>

static void vfork_parent_signal_handler(int sig, siginfo_t *siginfo, void *context) {
	task_vfork_end(task_self());
}

static void *vfork_child_task(void *arg) {
	struct pt_regs *ptregs = arg;

	ptregs_retcode_jmp(ptregs, 0);

	panic("vfork_child_task returning");
}

static void vfork_wait_signal_store(struct sigaction *ochildsa) {
	struct sigaction sa;

	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = vfork_parent_signal_handler;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGCHLD, &sa, ochildsa);
}

static void vfork_wait_signal_restore(const struct sigaction *ochildsa) {
	sigaction(SIGCHLD, ochildsa, NULL);
}

static void vfork_waiting(void) {
	struct sigaction ochildsa;
	struct task *child;
	struct task *parent;
	struct task_vfork *task_vfork;

	parent = task_self();

	task_vfork = task_resource_vfork(parent);
	child = task_table_get(task_vfork->child_pid);

	vfork_wait_signal_store(&ochildsa);
	{
		task_vfork_start(parent);

		task_start(child, vfork_child_task, &task_vfork->ptregs);

		while (SCHED_WAIT(!task_is_vforking(parent)));
	}
	vfork_wait_signal_restore(&ochildsa);

	longjmp(task_vfork->env, 1);

	panic("vfork_waiting returning");
}

int vfork_child_start(struct task *child) {
	struct task_vfork *task_vfork;

	task_vfork = task_resource_vfork(task_self());

	/* Allocate memory for the new stack */
	task_vfork->stack = sysmalloc(VFORK_CTX_STACK_LEN);

	if (!task_vfork->stack) {
		return -EAGAIN;
	}

	task_vfork->child_pid = child->tsk_id;

	/* Set new stack and go to vfork_waiting */
	if (!setjmp(task_vfork->env)) {
		CONTEXT_JMP_NEW_STACK(vfork_waiting,
				task_vfork->stack + VFORK_CTX_STACK_LEN);
	}

	/* current stack was broken, can't reach any old data */
	task_vfork = task_resource_vfork(task_self());

	sysfree(task_vfork->stack);

	ptregs_retcode_jmp(&task_vfork->ptregs, task_vfork->child_pid);

	panic("vfork_child_start returning");
	return -1;
}

void vfork_child_done(struct task *child, void * (*run)(void *), void *arg) {
	assert(run);

	if (child != task_self()) {
		task_start(child, run, arg);
	} else {
		run(arg);
	}
}


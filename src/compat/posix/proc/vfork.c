/**
 * @file
 *
 * @date Jul 12, 2014
 * @author Anton Bondarev
 */
#include <unistd.h>

#include <kernel/panic.h>
#include <kernel/task.h>
#include <kernel/task/resource/task_vfork.h>

#include <kernel/sched/sched_lock.h>

#include <hal/vfork.h>
#include <hal/ptrace.h>

extern int vfork_child_start(struct task *child);

static void *vfork_body_exit_stub(void *arg) {
	_exit(*((int*) arg));
}

void __attribute__((noreturn)) vfork_body(struct pt_regs *ptregs) {
	struct task *child;
	pid_t child_pid;
	struct task_vfork *task_vfork;
	int res;

	/* can vfork only in single thread application */
	assert(thread_self() == task_self()->tsk_main);

	/* create task description but not start its main thread */
	child_pid = task_prepare("");
	if (0 > child_pid) {
		/* error */
		ptregs_retcode_jmp(ptregs, child_pid);
		panic("vfork_body returning");
	}
	child = task_table_get(child_pid);
	/* save ptregs for parent return from vfork() */
	task_vfork = task_resource_vfork(child->parent);

	memcpy(&task_vfork->ptregs, ptregs, sizeof(task_vfork->ptregs));

	res = vfork_child_start(child);

	if (res < 0) {
		/* Could not start child process */
		ptregs_retcode(&task_vfork->ptregs, -1);

		vfork_child_done(child, vfork_body_exit_stub, &res);

		/* Just retutn to parent if vfork_child_done call was not successful */
		ptregs_jmp(&task_vfork->ptregs);
	}

	panic("vfork_body returning");
}

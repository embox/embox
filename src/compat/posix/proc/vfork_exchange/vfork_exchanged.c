/**
 * @file
 *
 * @date May 23, 2014
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <setjmp.h>

#include <kernel/panic.h>
#include <kernel/task.h>
#include <kernel/task/resource/task_vfork.h>

#include <kernel/sched/sched_lock.h>

#include <hal/vfork.h>
#include <hal/ptrace.h>

static void thread_set_task(struct thread *thread, struct task *task) {
	thread->task = task;
}

void vfork_child_done(struct task *child, void * (*run)(void *), void *arg) {
	struct task_vfork *vfork_data;

	if (!task_is_vforking(task_self())) {
		return;
	}

	vfork_data = task_resource_vfork(child);

	task_start(child, run, NULL);

	thread_set_task(thread_self(), child->parent);
	task_vfork_end(child);

	ptregs_retcode_jmp(&vfork_data->ptregs, child->tsk_id);
}

void __attribute__((noreturn)) vfork_body(struct pt_regs *ptregs) {
	struct task *child;
	pid_t child_pid;
	struct task_vfork *task_vfork;

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
	task_vfork = task_resource_vfork(child);

	memcpy(&task_vfork->ptregs, ptregs, sizeof(task_vfork->ptregs));

	sched_lock();
	{
		thread_set_task(thread_self(), child);

		/* mark as vforking */
		task_vfork_start(child);
	}
	sched_unlock();

	/* return from a child. The parent paused now */
	ptregs_retcode_jmp(ptregs, 0);
	panic("vfork_body returning");
}

void *task_exit_callback(void *arg) {
	_exit((int)arg);

	return arg;
}


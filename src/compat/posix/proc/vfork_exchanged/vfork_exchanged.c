	/**
	 * @file
 *
 * @date May 23, 2014
 * @author: Anton Bondarev
 */

#include <unistd.h>
#include <stdint.h>

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

	if (!task_is_vforking(child)) {
		return;
	}

	task_vfork_end(child);

	task_start(child, run, NULL);

	thread_set_task(thread_self(), child->parent);

	vfork_data = task_resource_vfork(child->parent);
	ptregs_retcode_jmp(&vfork_data->ptregs, child->tsk_id);
}

int vfork_child_start(struct task *child) {
	thread_set_task(thread_self(), child);

	/* mark as vforking */
	task_vfork_start(child);

	/* Restore values of the registers and return 0 */
	ptregs_retcode_jmp(&task_resource_vfork(child->parent)->ptregs, 0);

	panic("vfork_child_start returning");
	return -1;
}

void *task_exit_callback(void *arg) {
	_exit((intptr_t)arg);

	return arg;
}

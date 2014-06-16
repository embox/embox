/**
 * @file
 *
 * @brief
 *
 * @date 26.12.12
 * @author Alexander Kalmuk
 */

#include <unistd.h>

#include <kernel/sched.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/panic.h>
#include <kernel/task.h>
#include <hal/vfork.h>



static void *task_stub_exit(void *arg) {

	return arg;
}

void _exit(int status) {
	struct task *task;

	task = task_self();

	if (task_is_vforking(task)) {
		vfork_child_done(task, task_stub_exit, NULL);
	}

	task_start_exit();

	task_do_exit(task, TASKST_EXITED_MASK | (status & TASKST_EXITST_MASK));

	kill(task_get_id(task_get_parent(task)), SIGCHLD);

	task_finish_exit();

	panic("Returning from _exit");
}

/* stdlib */
void exit(int status) {
	_exit(status);
}

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

void _exit(int status) {

	task_start_exit();

	task_do_exit(task_self(), TASKST_EXITED_MASK | (status & TASKST_EXITST_MASK));

	kill(task_get_id(task_get_parent(task_self())), SIGCHLD);

	task_finish_exit();

	panic("Returning from _exit");
}

/* stdlib */
void exit(int status) {
	_exit(status);
}

/**
 * @file
 * @brief
 *
 * @date 21.03.2013
 * @author Anton Bulychev
 */

#include <errno.h>

#include <kernel/task.h>
#include <kernel/task/task_table.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/sched_lock.h>

int task_waitpid(unsigned int pid) {
	struct task *task;
	int res = ENOERR;

	sched_lock();
	{
		task = task_table_get(pid);

		if (task) {
			while (0 > sched_sleep_locked(task->wait_sq,
					SCHED_TIMEOUT_INFINITE));
		} else {
			res = -ENOENT;
		}
	}
	sched_unlock();

	return res;
}

static void task_wait_sq_deinit(struct task *task) {
	sched_wake_all(task->wait_sq);
}

static void task_wait_sq_init(struct task *task, void *_wait_sq) {
	task->wait_sq = _wait_sq;
	sleepq_init(task->wait_sq);
}

static const struct task_resource_desc waitpid_resource = {
	.init = task_wait_sq_init,
	.deinit = task_wait_sq_deinit,
	.resource_size = sizeof(struct sleepq)
};

TASK_RESOURCE_DESC(&waitpid_resource);

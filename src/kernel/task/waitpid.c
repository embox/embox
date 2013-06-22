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
#include <kernel/sched.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/sched/wait_queue.h>

int task_waitpid(pid_t pid) {
	struct task *task;
	int ret = 0;

	sched_lock();
	{
		task = task_table_get(pid);
		if (task == NULL) {
			ret = -ENOENT;
		}
		else {
			ret = wait_queue_wait_locked(task->waitq, SCHED_TIMEOUT_INFINITE);
		}
	}
	sched_unlock();

	return ret;
}

static void task_waitq_deinit(struct task *task) {
	wait_queue_notify_all(task->waitq);
}

static void task_waitq_init(struct task *task, void *_waitq) {
	task->waitq = _waitq;
	wait_queue_init(task->waitq);
}

static const struct task_resource_desc waitpid_resource = {
	.init = task_waitq_init,
	.deinit = task_waitq_deinit,
	.resource_size = sizeof(struct sleepq)
};

TASK_RESOURCE_DESC(&waitpid_resource);

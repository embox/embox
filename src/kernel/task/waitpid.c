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
#include <kernel/sched/waitq.h>

int task_waitpid(pid_t pid) {
	struct task *task, *parent;
	struct waitq_link wql;
	int ret = 0;

	waitq_link_init(&wql);

	sched_lock();
	{
		task = task_table_get(pid);
		parent = task->parent;
		if (!task) {
			ret = -ECHILD;
			goto out;
		}

		waitq_wait_prepare(task->waitq, &wql);

		sched_wait();

		ret = parent->child_err;

		/* no cleanup since task is dead already */
	}
out:
	sched_unlock();

	return ret;
}

static void task_waitq_deinit(struct task *task) {
	waitq_wakeup_all(task->waitq);
}

static void task_waitq_init(struct task *task, void *_waitq) {
	task->waitq = _waitq;
	waitq_init(task->waitq);
}

static const struct task_resource_desc waitpid_resource = {
	.init = task_waitq_init,
	.deinit = task_waitq_deinit,
	.resource_size = sizeof(struct waitq)
};

TASK_RESOURCE_DESC(&waitpid_resource);

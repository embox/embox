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
#include <kernel/task/resource/waitpid.h>

int task_waitpid(pid_t pid) {
	int ret;
	struct waitq_link wql;

	waitq_link_init(&wql);

	sched_lock();
	{
		struct task *task, *parent;

		task = task_table_get(pid);
		if (task == NULL) {
			ret = -ECHILD;
			goto out;
		}

		parent = task->parent;
		waitq_wait_prepare(task_resource_waitpid(task), &wql);

		sched_wait();

		assert(parent != NULL);
		ret = parent->child_err;

		/* no cleanup since task is dead already */
	}
out:
	sched_unlock();

	return ret;
}

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
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/sched/waitq.h>
#include <kernel/task/resource/errno.h>
#include <kernel/task/resource/waitpid.h>

int task_waitpid(pid_t pid) {
	int ret;
	struct task *tsk;
	struct waitq_link wql;

	waitq_link_init(&wql);

	sched_lock();
	{
		tsk = task_table_get(pid);
		if (tsk == NULL) {
			ret = -ECHILD;
			goto out;
		}

		waitq_wait_prepare(task_resource_waitpid(tsk), &wql);

		sched_wait();

		waitq_wait_cleanup(task_resource_waitpid(tsk), &wql);

		ret = *task_resource_errno(tsk);
		task_table_del(task_get_id(tsk));
		thread_delete(task_get_main(tsk));
	}
out:
	sched_unlock();

	return ret;
}

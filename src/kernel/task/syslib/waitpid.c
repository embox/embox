/**
 * @file
 * @brief
 *
 * @date 21.03.2013
 * @author Anton Bulychev
 */

#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <kernel/sched.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/sched/waitq.h>
#include <kernel/task.h>
#include <kernel/task/resource/errno.h>
#include <kernel/task/resource/waitpid.h>
#include <kernel/task/task_table.h>
#include <kernel/thread.h>
#include <kernel/thread/waitq.h>

pid_t task_waitpid(pid_t pid) {
	int ret;
	task_waitpid_posix(pid, &ret, 0);
	return ret;
}

static pid_t task_collect(pid_t target_pid, int *status) {
	struct dlist_head *child_link;

	dlist_foreach(child_link, &task_self()->child_list) {
		struct task *task = dlist_entry(child_link, struct task, child_lnk);
		pid_t child_pid = task_get_id(task);

		if ((target_pid == (pid_t)-1 || target_pid == child_pid)
		    && task->status & (TASKST_EXITED_MASK | TASKST_SIGNALD_MASK)) {
			if (status) {
				*status = task->status ^ TASKST_EXITED_MASK;
			}
			task_delete(task);
			return child_pid;
		}
	}

	return 0;
}

pid_t task_waitpid_posix(pid_t pid, int *status, int options) {
	pid_t ret;

	if (options & WNOHANG) {
		return task_collect(pid, status);
	}

	while (0 > WAITQ_WAIT(task_resource_waitpid(task_self()),
	           (ret = task_collect(pid, status)))) {}

	return ret;
}

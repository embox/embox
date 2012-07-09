/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.06.2012
 */

#include <kernel/task.h>
#include <kernel/task/errno.h>

int * task_self_errno_ptr(void) {
	return & task_self()->err;
}

int task_errno(struct task *task) {
	return task->err;
}

void task_errno_set(struct task *task, int errno) {
	task->err = errno;
}

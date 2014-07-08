/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    26.06.2014
 */

#include <kernel/task.h>
#include <kernel/task/task_vforking.h>
#include <kernel/task/resource/task_vfork.h>

struct task *task_vforking(struct task *task) {
#if 0
	struct task *tk;

	if (!task_is_vforking(task)) {
		tk = task;
	} else {
		struct task_vfork *task_vfork;

		task_vfork = task_resource_vfork(task);
		tk = task_vfork->vforked_task;
	}

	return tk;
#endif
	return task;
}



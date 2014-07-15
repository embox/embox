/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    04.06.2014
 */

#include <kernel/task/resource.h>
#include <kernel/task.h>

void task_init(struct task *tsk, int id, struct task *parent,
		const char *name, struct thread *main_thread,
		task_priority_t priority) {
	assert(tsk == task_kernel_task());
	assert(id == task_get_id(tsk));
	assert(0 == strcmp(name, task_get_name(tsk)));
	assert(main_thread == task_get_main(tsk));
	assert(TASK_PRIORITY_DEFAULT == task_get_priority(tsk));

	if (main_thread != NULL) { /* check for thread.NoThreads module */
		main_thread->task = tsk;
	}

	task_resource_init(tsk);
}


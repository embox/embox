/**
 * @file
 *
 * @brief
 *
 * @date 06.12.2011
 * @author Anton Bondarev
 */

#include <string.h>
#include <kernel/task.h>
#include <util/binalign.h>
#include <kernel/task/resource.h>
#include <kernel/thread.h>

struct task * task_init(void *space, size_t size,
		int id, const char *name, struct thread *main_thread,
		task_priority_t priority) {
	struct task *task;
	size_t task_off, task_sz;

	task = (struct task *)binalign_bound((uintptr_t)space, 4);
	task_off = (void *)task - space;
	task_sz = sizeof *task + TASK_RESOURCE_SIZE;

	if (size < task_off + task_sz) {
		return NULL;
	}

	task->tsk_id = id;

	strncpy(task->tsk_name, name, sizeof task->tsk_name - 1);
	task->tsk_name[sizeof task->tsk_name - 1] = '\0';

	task->tsk_main = main_thread;
	main_thread->task = task;

	task->tsk_priority = priority;

	task->tsk_clock = 0;

	task_resource_init(task);

	return task;
}

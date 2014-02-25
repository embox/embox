/**
 * @file
 *
 * @brief
 *
 * @date 06.12.2011
 * @author Anton Bondarev
 */

#include <string.h>
#include <util/dlist.h>
#include <kernel/task.h>
#include <util/binalign.h>
#include <kernel/task/resource.h>

struct task * task_init(void *space, size_t size,
		const char *name) {
	struct task *task;
	size_t task_off, task_sz;

	task = (struct task *)binalign_bound((uintptr_t)space, 4);
	task_off = (void *)task - space;
	task_sz = sizeof task + task_resource_size();

	if (size < task_off + task_sz) {
		return NULL;
	}

	memset(space, 0, task_sz);

	strncpy(task->task_name, name, sizeof task->task_name - 1);
	task->task_name[sizeof task->task_name - 1] = '\0';

	dlist_head_init(&task->task_link);

	dlist_init(&task->children_tasks);

	task->parent = NULL;

	task->priority = TASK_PRIORITY_DEFAULT;

	task->resources = task + 1;
	task_resource_init(task, task->resources);

	task_thread_key_init(task);

	/* FIXME: This should be only in SMP */
	task->affinity = TASK_AFFINITY_DEFAULT;

	task->child_err = 0;

	return task;
}

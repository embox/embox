/**
 * @file
 *
 * @date May 1, 2020
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <assert.h>
#include <limits.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/atexit.h>

TASK_RESOURCE_DEF(task_atexit_desc, struct task_atexit);

static void task_atexit_init(const struct task *task, void *atexit_space) {
	struct task_atexit *task_atexit;
	int i;

	assert(atexit_space == task_resource_atexit(task));

	task_atexit = atexit_space;

	for (i = 0; i < ATEXIT_MAX; ++i) {
		task_atexit->atexit_func[i] = NULL;
	}
	task_atexit->slot = 0;
}

static int task_atexit_inherit(const struct task *task,
		const struct task *parent) {
	struct task_atexit *task_atexit;
	size_t i;

	task_atexit = task_resource_atexit(task);
	assert(task_atexit != NULL);

	for (i = 0; i < ATEXIT_MAX; ++i) {
		task_atexit->atexit_func[i] = NULL;
	}
	task_atexit->slot = 0;

	return 0;
}

static size_t task_atexit_offset;

static const struct task_resource_desc task_atexit_desc = {
	.init = task_atexit_init,
	.inherit = task_atexit_inherit,
	.resource_size = sizeof(struct task_atexit),
	.resource_offset = &task_atexit_offset
};

struct task_atexit * task_resource_atexit(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_atexit_offset;
}

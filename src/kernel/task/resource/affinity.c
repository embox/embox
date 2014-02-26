/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    25.06.2012
 */

#include <assert.h>
#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/affinity.h>
#include <stddef.h>

TASK_RESOURCE_DESC(task_affinity_res);

static size_t task_affinity_res_offset;

static void task_affinity_init(const struct task *task,
		void *affinity_space) {
	unsigned int *affinity;

	assert(affinity_space == task_resource_affinity(task));

	affinity = affinity_space;
	assert(affinity != NULL);

	*affinity = TASK_AFFINITY_DEFAULT;
}

static const struct task_resource_desc task_affinity_res = {
	.init = task_affinity_init,
	.resource_size = sizeof(unsigned int),
	.resource_offset = &task_affinity_res_offset
};

unsigned int * task_resource_affinity(const struct task *task) {
	assert(task != NULL);
	assert(task->resources != NULL);
	return task->resources + task_affinity_res_offset;
}

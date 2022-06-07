/**
 * @file
 * @brief
 *
 * @author  Anton Ostrouhhov
 * @date    03.05.2022
 */

#include <assert.h>

#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/lkl_resources.h>

TASK_RESOURCE_DEF(task_lkl_resources_desc, struct lkl_resources);

static size_t task_lkl_resources_offset;

struct lkl_resources *task_lkl_resources(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_lkl_resources_offset;
}

static void task_lkl_resources_init(const struct task *task) {
	task_lkl_resources(task)->lkl_allowed = 0;
	task_lkl_resources(task)->lkl_tls_key = NULL;
}

static const struct task_resource_desc task_lkl_resources_desc = {
	.init = task_lkl_resources_init,
	.resource_size = sizeof(struct lkl_resources),
	.resource_offset = &task_lkl_resources_offset
};
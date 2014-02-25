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
#include <kernel/task/resource/errno.h>
#include <stddef.h>

TASK_RESOURCE_DESC(task_errno_res);

static size_t task_errno_res_offset;

static const struct task_resource_desc task_errno_res = {
	.resource_size = sizeof(int),
	.resource_offset = &task_errno_res_offset
};

int * task_resource_errno(const struct task *task) {
	assert(task != NULL);
	assert(task->resources != NULL);
	return task->resources + task_errno_res_offset;
}

int * task_self_resource_errno(void) {
	assert(task_self() != NULL);
	assert(task_self()->resources != NULL);
	return task_self()->resources + task_errno_res_offset;
}

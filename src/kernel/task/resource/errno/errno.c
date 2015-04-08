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

TASK_RESOURCE_DEF(task_errno_desc, int);

static size_t task_errno_offset;

static const struct task_resource_desc task_errno_desc = {
	.resource_size = sizeof(int),
	.resource_offset = &task_errno_offset
};

int *task_resource_errno(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_errno_offset;
}

int *task_self_resource_errno(void) {
	return task_resource_errno(task_self());
}

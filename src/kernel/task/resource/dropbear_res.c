/**
 * @file    dropbear.c - resources for dropbear session
 * @brief   claim resource space for dropbear server session
 *
 * @date    24.07.2023
 */

#include <assert.h>
#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/dropbear_res.h>
#include <stddef.h>

TASK_RESOURCE_DEF(task_dropbear_desc, struct drpbr_ses);

static size_t task_dropbear_offset;

static const struct task_resource_desc task_dropbear_desc = {
	.resource_size = sizeof(struct drpbr_ses),
	.resource_offset = &task_dropbear_offset
};

struct drpbr_ses *task_resource_dropbear(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_dropbear_offset;
}

struct drpbr_ses *task_self_resource_dropbear(void) {
	return task_resource_dropbear(task_self());
}

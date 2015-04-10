/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.02.2013
 */

#include <assert.h>
#include <kernel/task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/u_area.h>
#include <stddef.h>
#include <string.h>

TASK_RESOURCE_DEF(task_u_area_desc, struct task_u_area);

static void task_u_area_init(const struct task *task,
		void *u_area_space) {
	struct task_u_area *u_area;

	assert(u_area_space == task_resource_u_area(task));

	u_area = u_area_space;
	assert(u_area != NULL);

	memset(u_area, 0, sizeof *u_area);
}

static int task_u_area_inherit(const struct task *task,
		const struct task *parent) {
	struct task_u_area *task_u_area, *parent_u_area;

	task_u_area = task_resource_u_area(task);
	assert(task_u_area != NULL);

	parent_u_area = task_resource_u_area(parent);
	assert(parent_u_area != NULL);

	memcpy(task_u_area, parent_u_area, sizeof *task_u_area);

	return 0;
}

static size_t task_u_area_offset;

static const struct task_resource_desc task_u_area_desc = {
	.init = task_u_area_init,
	.inherit = task_u_area_inherit,
	.resource_size = sizeof(struct task_u_area),
	.resource_offset = &task_u_area_offset
};

struct task_u_area * task_resource_u_area(const struct task *task) {
	assert(task != NULL);
	return (void *)task->resources + task_u_area_offset;
}

/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.02.2013
 */

#include <string.h>
#include <kernel/task.h>

#include <kernel/task/u_area.h>

static void u_area_init(struct task *task, void* _u_area) {
	struct task_u_area *u_area = _u_area;
	memset(u_area, 0, sizeof(struct task_u_area));
	task->u_area = u_area;
}

static void u_area_inherit(struct task *task, struct task *parent) {
	memcpy(task->u_area, parent->u_area, sizeof(struct task_u_area));
}

static void u_area_deinit(struct task *task) {
}

static const struct task_resource_desc u_area_res = {
	.init = u_area_init,
	.inherit = u_area_inherit,
	.deinit = u_area_deinit,
	.resource_size = sizeof(struct task_u_area),
};

TASK_RESOURCE_DESC(&u_area_res);

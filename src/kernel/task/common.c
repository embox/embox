/**
 * @file
 *
 * @brief
 *
 * @date 06.12.2011
 * @author Anton Bondarev
 */
#include <embox/unit.h>
#include <kernel/task.h>
#include "common.h"

EMBOX_UNIT_INIT(tasks_init);

static struct task kernel_task;

ARRAY_SPREAD_DEF(const struct task_resource_desc *, task_resource_desc_array);

static void task_root_init(struct task *task) {
	const struct task_resource_desc *res_desc;

	INIT_LIST_HEAD(&task->threads);
	INIT_LIST_HEAD(&task->children);
	INIT_LIST_HEAD(&task->link);

	task->parent = NULL;

	task_resource_foreach(res_desc) {
		res_desc->init(task);
	}

}

struct task *task_kernel_task(void) {
	return &kernel_task;
}

static int tasks_init(void) {

	task_root_init(&kernel_task);

	return 0;
}

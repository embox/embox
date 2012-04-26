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
#include <diag/diag_device.h> //TODO
#include "index_desc.h"

EMBOX_UNIT_INIT(tasks_init);

static struct task default_task;


void task_root_init(struct task *new_task) {
	INIT_LIST_HEAD(&new_task->threads);
	INIT_LIST_HEAD(&new_task->children);
	INIT_LIST_HEAD(&new_task->link);

	new_task->parent = NULL;

	res_init(task_get_resources(new_task));
}

struct task *task_default_get(void) {
	return &default_task;
}

static int tasks_init(void) {
	FILE* file = diag_device_get();
	struct task_resources *res = task_get_resources(&default_task);
	struct idx_desc *cidx = task_idx_desc_alloc(TASK_IDX_TYPE_FILE, file);

	task_root_init(&default_task);

	task_res_idx_set(res, 0, cidx);
	task_res_idx_set(res, 1, cidx);
	task_res_idx_set(res, 2, cidx);

	return 0;
}

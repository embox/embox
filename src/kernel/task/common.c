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
#include "index_desc.h"

EMBOX_UNIT_INIT(tasks_init);

static struct task kernel_task;

static void task_root_init(struct task *new_task) {
	INIT_LIST_HEAD(&new_task->threads);
	INIT_LIST_HEAD(&new_task->children);
	INIT_LIST_HEAD(&new_task->link);

	new_task->parent = NULL;

	task_idx_table_init(task_idx_table(new_task));
}

struct task *task_kernel_task(void) {
	return &kernel_task;
}

static int tasks_init(void) {

	task_root_init(&kernel_task);

	return 0;
}

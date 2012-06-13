/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <kernel/thread/api.h>
#include <mem/objalloc.h>
#include <kernel/task.h>
#include "index_desc.h"

#include <embox/unit.h> /* For options */

OBJALLOC_DEF(task_pool, struct task, OPTION_GET(NUMBER, tasks_quantity));

static void task_init(struct task *new_task, struct task *parent) {
	struct task_resources *res = task_get_resources(new_task);
	struct task_resources *par_res = task_get_resources(parent);

	new_task->parent = parent;

	task_res_idx_init(res);

	for (int i = 0; i < CONFIG_TASKS_RES_QUANTITY; i++) {
		if (!task_res_idx_is_binded(par_res, i)) {
			continue;
		}

		task_res_idx_set(res, i, task_res_idx_get(par_res, i));
	}

	INIT_LIST_HEAD(&new_task->threads);

	list_add(&new_task->link, &parent->children);
}

int task_create(struct task **new, struct task *parent) {
	assert(parent != NULL);

	if (NULL == (*new = (struct task *) objalloc(&task_pool))) {
		return -ENOMEM;
	}

	task_init(*new, parent);

	return ENOERR;
}

struct task *task_self(void) {
	return thread_self()->task;
}

int task_delete(struct task *tsk) {
	return 0;
}

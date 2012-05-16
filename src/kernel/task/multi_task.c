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
#include <kernel/file.h>
#include "index_desc.h"

#include <embox/unit.h>

OBJALLOC_DEF(task_pool, struct task, OPTION_GET(NUMBER, tasks_quantity));

static void task_init(struct task *new_task, struct task *parent) {
	//struct __fd_list *fdl, *par_fdl;
	struct idx_desc *par_idx_desc;
	struct task_resources *res = task_get_resources(new_task);
	struct task_resources *par_res = task_get_resources(parent);
	new_task->parent = parent;

	res_init(res);

	for (int i = 0; i < CONFIG_TASKS_RES_QUANTITY; i++) {
		if (!task_res_idx_is_binded(par_res, i)) {
			continue;
		}

		par_idx_desc = task_res_idx_get(task_get_resources(parent), i);

		task_res_idx_set(res, i, par_idx_desc);
		task_idx_desc_link_count_add(par_idx_desc, 1);
	}

	INIT_LIST_HEAD(&new_task->threads);

	list_add(&new_task->link, &parent->children);
}

int task_create(struct task **new, struct task *parent) {

	if (NULL == (*new = (struct task *) objalloc(&task_pool))) {
		return -ENOMEM;
	}

	if (parent != NULL) {
		task_init(*new, parent);
	} else {
		task_root_init(*new);
	}

	return ENOERR;
}

struct task *task_self(void) {
	return thread_self()->task;
}

int task_delete(struct task *tsk) {
	return 0;
}

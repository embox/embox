/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <util/array.h>
#include <kernel/thread/api.h>
#include <mem/objalloc.h>
#include <kernel/task.h>
#include <fs/file.h>
#include "index_desc.h"

OBJALLOC_DEF(task_pool, struct task, CONFIG_TASKS_N_MAX);

static void task_init(struct task *new_task, struct task *parent) {
	//struct __fd_list *fdl, *par_fdl;
	struct idx_desc *par_idx_desc;
	struct task_resources *res = task_get_resources(new_task);
	new_task->parent = parent;

	res_init(res);

	for (int i = 0; i < CONFIG_TASKS_RES_QUANTITY; i++) {
		par_idx_desc = task_res_idx_get(task_get_resources(parent), i);

		if (task_idx_desc_get_res(par_idx_desc) == NULL) {
			continue;
		}

		task_res_idx_set(res, i, par_idx_desc);
		par_idx_desc->link_count++;
	}

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

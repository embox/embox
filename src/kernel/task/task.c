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
	struct __fd_list *fdl, *par_fdl;
	struct task_resources *res = task_get_resources(new_task);
	new_task->parent = parent;

	fd_list_init(&new_task->resources);

	for (int i = 0; i < CONFIG_TASKS_FILE_QUANTITY; i++) {
		par_fdl = &task_get_resources(parent)->fds[i];

		if (par_fdl->file != NULL) {
			continue;
		}

		fdl = &res->fds[task_idx_alloc_res(TASK_IDX_TYPE_FILE, res)];
		fdl->file = par_fdl->file;
		list_add(&fdl->link, &par_fdl->link);
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

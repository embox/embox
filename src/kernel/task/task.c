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
#include "task_common.h"

OBJALLOC_DEF(task_pool, struct task, CONFIG_TASKS_N_MAX);

static int desc2idx(struct task *parent, struct __fd_list *desc) {
	int i;
	for(i = 0; i < ARRAY_SIZE(parent->fd_array.fds); i ++) {
		if(parent->fd_array.fds[i].file == desc->file) {
			return i;
		}
	}
	return -1;
}

static void task_init(struct task *new_task, struct task *parent) {
	struct list_head *it;
	struct __fd_list *fdl;
	int fd;
	new_task->parent = parent;

	fd_list_init(&new_task->fd_array);

	list_for_each(it, &parent->fd_array.opened_fds) {
		fdl = (struct __fd_list *) it;
		if (-1 == (fd = desc2idx(parent, fdl))) {
			continue;
		}
		//fd = ((int) fdl - (int)parent->fd_array.fds) / sizeof(struct __fd_list);

		new_task->fd_array.fds[fd].file = parent->fd_array.fds[fd].file;

		list_move(&new_task->fd_array.fds[fd].link, &new_task->fd_array.opened_fds);

		list_add(&new_task->fd_array.fds[fd].file_link, &fdl->file_link);
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

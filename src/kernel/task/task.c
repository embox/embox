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
#include <fs/file.h>
#include <embox/unit.h>
#include <diag/diag_device.h>
EMBOX_UNIT_INIT(tasks_init);

OBJALLOC_DEF(task_pool, struct task, CONFIG_TASKS_N_MAX);

static struct task default_task;

static void fd_list_init(struct __fd_array *fd_array) {
	INIT_LIST_HEAD(&fd_array->free_fds);
	INIT_LIST_HEAD(&fd_array->opened_fds);

	for (int i = 0; i < FD_N_MAX; i++) {
		INIT_LIST_HEAD(&fd_array->fds[i].link);
		list_add_tail(&fd_array->fds[i].link,
						&fd_array->free_fds);
	}
}

static void task_root_init(struct task *new_task) {
	INIT_LIST_HEAD(&new_task->threads);
	INIT_LIST_HEAD(&new_task->children);
	INIT_LIST_HEAD(&new_task->link);

	new_task->parent = NULL;

	fd_list_init(&new_task->fd_array);
}

static void task_init(struct task *new_task, struct task *parent) {

	new_task->parent = parent;

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

struct task *task_default_get(void) {
	return &default_task;
}

int task_delete(struct task *tsk) {
	return 0;
}

static int tasks_init(void) {
	FILE* file = diag_device_get();

	task_root_init(&default_task);

	default_task.fd_array.fds[0].file = file;
	default_task.fd_array.fds[1].file = file;
	default_task.fd_array.fds[2].file = file;

	return 0;
}

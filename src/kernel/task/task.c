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

EMBOX_UNIT_INIT(tasks_init);

#define FD_N_MAX 16

struct __fd_array {
	struct __fd_list fds[FD_N_MAX];
};

//OBJALLOC_DEF(fds_arrays, struct __fd_array, CONFIG_TASKS_N_MAX);

OBJALLOC_DEF(task_pool, struct task, CONFIG_TASKS_N_MAX);

static struct task default_task;

static struct task *task_alloc(void) {
	struct task *new_task =  (struct task *) objalloc(&task_pool);

	return new_task;
}

static void task_init(struct task *new_task, struct task *parent) {

	INIT_LIST_HEAD(&new_task->threads);
	INIT_LIST_HEAD(&new_task->child_tasks);
	INIT_LIST_HEAD(&new_task->child_link);
	INIT_LIST_HEAD(&new_task->free_fds);
	INIT_LIST_HEAD(&new_task->opened_fds);

	new_task->parent = parent;

	for (int i = 0; i < FD_N_MAX; i++) {
		new_task->fds[i].fd = i;
		INIT_LIST_HEAD(&new_task->fds[i].list_hnd);
		list_add_tail(&new_task->fds[i].list_hnd,
				&new_task->free_fds);
	}

	if (parent == NULL) {
		return;
	}

	list_add(&new_task->child_link, &parent->child_tasks);

}

int task_create(struct task **new, struct task *parent) {
	struct task *new_task;

	*new = (struct task *) NULL;

	new_task = task_alloc();
	if (new_task == NULL) {
		return -ENOMEM;
	}

	task_init(new_task, parent);

	*new = new_task;
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

	task_init(&default_task, NULL);

	return 0;
}

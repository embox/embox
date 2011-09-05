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
#include <kernel/thread/task.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(tasks_init);

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

	new_task->parent = parent;

	if (parent != NULL) {
		list_add(&new_task->child_link, &parent->child_tasks);
	}
}

int task_create(struct task **new, struct task *parent) {
	struct task *new_task;
	*new = (struct task *) NULL;

	new_task = task_alloc();
	if (new_task == NULL) {
		return -ENOMEM;
	}

	task_init(new_task, task_self());

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

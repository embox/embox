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
#include "common.h"

#include <embox/unit.h> /* For options */

OBJALLOC_DEF(task_pool, struct task, OPTION_GET(NUMBER, tasks_quantity));

static void task_init(struct task *task, struct task *parent) {
	const struct task_resource_desc *res_desc;
	task->parent = parent;

	INIT_LIST_HEAD(&task->threads);

	list_add(&task->link, &parent->children);

	task_resource_foreach(res_desc) {
		res_desc->inherit(task, parent);
	}

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

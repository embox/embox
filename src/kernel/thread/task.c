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

extern int thread_create_task(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg, struct task *tsk);

OBJALLOC_DEF(task_pool, struct task, CONFIG_TASKS_N_MAX);


struct task *task_alloc(void) {
	struct task *new_task =  (struct task *) objalloc(&task_pool);

	return new_task;
}

void task_init(struct task *new_task, struct task *parent) {
	INIT_LIST_HEAD(&new_task->threads);
	INIT_LIST_HEAD(&new_task->child_tasks);
	INIT_LIST_HEAD(&new_task->child_link);

	new_task->parent = parent;

	list_add(&new_task->child_link, &parent->child_tasks);
}

int task_create(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg) {
	struct thread *new_thread;
	int res = 0;

	struct task *new_task = task_alloc();
	if (new_task == NULL) {
		return -ENOMEM;
	}

	task_init(new_task, task_self());

	if ((res = thread_create_task(&new_thread, flags, run, arg, new_task)) != 0) {
		return res;
	}

	return ENOERR;
}

struct task *task_self(void) {
	return thread_self()->task;
}

int task_delete(struct task *tsk) {
	return 0;
}


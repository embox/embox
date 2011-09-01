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
//#include <util/list.h>


OBJALLOC_DEF(task_pool, struct task, CONFIG_TASKS_N_MAX);


static struct task *task_alloc(void) {
	struct task *new_task =  (struct task *) objalloc(&task_pool);


	//dlist_init(new_task->threads);
	//dlist_init(new_task->child_tasks);
	new_task->parent = task_self();
	return new_task;
}

int task_create(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg) {
	struct thread *new_thread;
	int res = 0;

	struct task *new_task = task_alloc();
	if (new_task == NULL) {
		return -ENOMEM;
	}
	if ((res = thread_create(&new_thread, flags, run, arg)) != 0) {
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


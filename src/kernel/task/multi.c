/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <embox/unit.h>

#include <errno.h>

#include <kernel/task/task_table.h>
#include <kernel/thread/api.h>
#include <mem/misc/pool.h>
#include <kernel/task.h>
#include <kernel/panic.h>
#include <kernel/critical.h>

#include "common.h"

EMBOX_UNIT_INIT(unit_init);

typedef void *(*run_fn)(void *);

/* used for passing params from caller to thread creator*/
struct task_creat_param {
	run_fn run;
	void *arg;
};

/* Maximum simultaneous creating task number */
#define SIMULTANEOUS_TASK_CREAT 10

/* struct's livecycle is short: created in new_task,
 * freed at first in new task's thread */
POOL_DEF(creat_param, struct task_creat_param, SIMULTANEOUS_TASK_CREAT);

static void *task_trampoline(void *arg);
static void thread_set_task(struct thread *t, struct task *tsk);
static void task_init_parent(struct task *task, struct task *parent);

int new_task(void *(*run)(void *), void *arg) {
	struct task_creat_param *param;
	struct thread *thd = NULL;
	struct task *self_task = NULL;
	int res = 0;
	const int task_sz = task_resource_sum_size() + sizeof(struct task);

	sched_lock();
	{
		param = (struct task_creat_param *) pool_alloc(&creat_param);
		if (!param) {
			sched_unlock();
			return -EAGAIN;
		}

		if (!task_table_has_space()) {
			pool_free(&creat_param, param);
			sched_unlock();
			return -ENOMEM;
		}

		param->run = run;
		param->arg = arg;

		/*
		 * Thread does not run until we go through sched_unlock()
		 */
		if (0 != (res = thread_create(&thd, THREAD_FLAG_PRIORITY_INHERIT,
				task_trampoline, param))) {
			sched_unlock();
			return res;
		}

		/* alloc space for task & resources on top of created thread's stack */

		if ((self_task = task_init(thd->stack, thd->stack_sz)) == NULL) {
			thread_terminate(thd);
			thread_detach(thd);
			pool_free(&creat_param, param);
			sched_unlock();
			return -EPERM;
		}

		self_task->main_thread = thd;

		thd->stack += task_sz;
		thd->stack_sz -= task_sz;

		context_set_stack(&thd->context, thd->stack + thd->stack_sz);

		/* init new task */

		task_init_parent(self_task, task_self());

		thread_set_task(thd, self_task);

		thread_detach(thd);

		res = task_table_add(self_task);
	}
	sched_unlock();

	return res;
}

struct task *task_self(void) {
	return thread_self()->task;
}

int task_notify_switch(struct thread *prev, struct thread *next) {
	task_notifing_resource_hnd notify_res;
	int res;

	if (prev->task == next->task) {
		return 0;
	}

	task_notifing_resource_foreach(notify_res) {
		if (0 != (res = notify_res(prev, next))) {
			return res;
		}
	}

	return 0;
}

static void thread_set_task(struct thread *t, struct task *tsk) {
	t->task = tsk;
	list_move_tail(&t->task_link, &tsk->threads);
}

static void task_init_parent(struct task *task, struct task *parent) {
	const struct task_resource_desc *res_desc;
	task->parent = parent;

	INIT_LIST_HEAD(&task->threads);

	list_add(&task->link, &parent->children);

	task_resource_foreach(res_desc) {
		if (res_desc->inherit) {
			res_desc->inherit(task, parent);
		}
	}

}

void __attribute__((noreturn)) task_exit(void *res) {
	struct task *task = task_self();
	struct thread *thread, *next;
	const struct task_resource_desc *res_desc;

	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		/* Deinitialize all resources */
		task_resource_foreach(res_desc) {
			if (res_desc->deinit) {
				res_desc->deinit(task);
			}
		}

		/* Remove us from list of tasks */
		list_del(&task->link);

		/* Release our task id */
		task_table_del(task->tid);

		/*
		 * Terminate all threads except main thread. If we terminate current
		 * thread then until we in sched_lock() we continue processing
		 * and our thread structure is not freed.
		 */
		list_for_each_entry_safe(thread, next, &task->threads, task_link) {
			if (thread == task->main_thread) {
				continue;
			}

			thread_terminate(thread);
		}

		/* At the end terminate main thread */
		thread_terminate(task->main_thread);
	}
	sched_unlock();

	/* NOTREACHED */
	panic("Returning from task_exit()");
}

static void *task_trampoline(void *arg) {
	struct task_creat_param *param = (struct task_creat_param *) arg;
	void *run_arg = param->arg;
	run_fn run = param->run;
	void *res = NULL;

	sched_lock();
	{
		pool_free(&creat_param, param);
	}
	sched_unlock();

	res = run(run_arg);
	task_exit(res);

	/* NOTREACHED */

	panic("Returning from task_trampoline()");

	return res;
}

int unit_init(void) {
	task_table_add(task_kernel_task());

	return 0;
}

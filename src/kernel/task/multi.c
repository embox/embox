/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <errno.h>
#include <kernel/task/task_table.h>
#include <kernel/thread/api.h>
#include <mem/misc/pool.h>
#include <kernel/task.h>
#include <kernel/panic.h>
#include "common.h"

#include <hal/mm/mmu_core.h>

#include <embox/unit.h> /* For options */

typedef void *(*run_fn)(void *);

/* used for passing params from caller to thread creator*/
struct task_creat_param {
	run_fn run;
	void *arg;
};

/* Maximum simultaneous creating task number */
#define SIMULTANEOUS_TASK_CREAT 5

/* struct's livecycle is short: created in new_task,
 * freed at first in new task's thread */
POOL_DEF(creat_param, struct task_creat_param, SIMULTANEOUS_TASK_CREAT);

static void *task_trampoline(void *arg);
static void thread_set_task(struct thread *t, struct task *tsk);
static void task_init_parent(struct task *task, struct task *parent);

int new_task(void *(*run)(void *), void *arg) {
	struct task_creat_param *param = (struct task_creat_param *) pool_alloc(&creat_param);
	struct thread *thd = NULL;
	struct task *self_task = NULL;
	int res = 0;
	const int task_sz = task_resource_sum_size() + sizeof(struct task);

	if (!param) {
		return -EAGAIN;
	}

	if (! task_table_has_space()) {
		pool_free(&creat_param, param);
		return -ENOMEM;
	}

	param->run = run;
	param->arg = arg;

	if (0 != (res = thread_create(&thd, THREAD_FLAG_SUSPENDED, task_trampoline, param))) {
		return res;
	}

	/* alloc space for task & resources on top of created thread's stack */

	self_task = task_init(thd->stack);

	thd->stack += task_sz;
	thd->stack_sz -= task_sz;

	context_set_stack(&thd->context, thd->stack + thd->stack_sz);

	/* init new task */

	task_init_parent(self_task, task_self());

	thread_set_task(thd, self_task);

	thread_detach(thd);

	thread_launch(thd);

	return task_table_add(self_task);

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

	switch_mm(prev->task->ctx, next->task->ctx);

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
		res_desc->inherit(task, parent);
	}

}

void __attribute__((noreturn)) task_exit(void *res) {
	struct task *this_task = task_self();
	struct thread *thread;
	const struct task_resource_desc *res_desc;

	list_del(&this_task->link);

	sched_lock();

	/* suspend everything except us */
	list_for_each_entry(thread, &this_task->threads, task_link) {
		if (thread == thread_self()) {
			continue;
		}
		thread_terminate(thread);
	}

	sched_unlock();

	task_resource_foreach(res_desc) {
		res_desc->deinit(this_task);
	}

	task_table_del(this_task->tid);

	list_for_each_entry(thread, &this_task->threads, task_link) {
		if (thread == thread_self()) {
			continue;
		}
		/*thread_delete(thread);*/
	}

	thread_exit(res);
}

static void *task_trampoline(void *arg) {
	struct task_creat_param *param = (struct task_creat_param *) arg;
	void *run_arg = param->arg;
	run_fn run = param->run;
	void *res = NULL;

	pool_free(&creat_param, param);

	res = run(run_arg);

	task_exit(res);

	/* NOTREACHED */

	panic("Returning from task_trampoline()");

	return res;
}


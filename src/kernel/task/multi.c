/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <embox/unit.h>

#include <string.h>
#include <errno.h>
#include <assert.h>

#include <kernel/task/task_table.h>
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <mem/misc/pool.h>
#include <kernel/task.h>
#include <kernel/panic.h>
#include <kernel/critical.h>
#include <util/idx_table.h>

#include <err.h>

#include "common.h"

#define TASK_QUANT OPTION_GET(NUMBER, tasks_quantity)

UTIL_IDX_TABLE_DEF(struct task *, task_table, TASK_QUANT);

EMBOX_UNIT_INIT(unit_init);

typedef void *(*run_fn)(void *);

/* used for passing params from caller to thread creator*/
struct task_creat_param {
	run_fn run;
	void *arg;
};

/* Maximum simultaneous creating task number */
#define SIMULTANEOUS_TASK_CREAT 10

/* struct's life cycle is short: created in new_task,
 * freed at first in new task's thread */
POOL_DEF(creat_param, struct task_creat_param, SIMULTANEOUS_TASK_CREAT);

static void *task_trampoline(void *arg);
static int task_init_parent(struct task *task, struct task *parent);

int new_task(const char *name, void *(*run)(void *), void *arg) {
	struct task_creat_param *param;
	struct thread *thd = NULL;
	struct task *self_task = NULL;
	int res = 0;
	const int task_sz = task_size();
	void *addr;

	sched_lock();
	{
		param = (struct task_creat_param *) pool_alloc(&creat_param);
		if (!param) {
			res = -EAGAIN;
			goto out_unlock;
		}

		if (!task_table_has_space()) {
			res = -ENOMEM;
			goto out_poolfree;
		}

		param->run = run;
		param->arg = arg;

		/*
		 * Thread does not run until we go through sched_unlock()
		 */
		thd = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED,
				task_trampoline, param);
		if (0 != err(thd)) {
			goto out_poolfree;
		}

		addr = thread_stack_get(thd);

		if (thread_stack_reserved(thd, task_sz) < 0) {
			panic("Too small thread stack size");
		}

		if ((self_task = task_init(addr, task_sz)) == NULL) {
			res = -EPERM;
			goto out_threadfree;
		}

		thd->task = self_task;
		self_task->main_thread = thd;


		self_task->per_cpu = 0;

		self_task->priority = task_self()->priority;

		/* initialize the new task */
		if (strlen(name) > MAX_TASK_NAME_LEN) {
			res = -EPERM;
			goto out_threadfree;
		}

		strncpy(self_task->task_name, name, sizeof(self_task->task_name) - 1);

		if ((res = task_table_add(self_task)) < 0) {
			goto out_threadfree;
		}

		res = task_init_parent(self_task, task_self());
		if (res != 0) {
			goto out_tablefree;
		}

		thread_set_priority(thd,
				sched_priority_thread(task_self()->priority,
						thread_priority_get(thread_self())));
		thread_detach(thd);
		thread_launch(thd);

		res = self_task->tid;

		goto out_unlock;

out_tablefree:
		task_table_del(self_task->tid);

out_threadfree:
		thread_terminate(thd);

out_poolfree:
		pool_free(&creat_param, param);

	}
out_unlock:
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

static int task_init_parent(struct task *task, struct task *parent) {
	int ret;
	const struct task_resource_desc *res_desc, *failed;

	task->parent = parent;

	task_resource_foreach(res_desc) {
		if (res_desc->inherit) {
			ret = res_desc->inherit(task, parent);
			if (ret != 0) {
				goto out_free_resource;
			}
		}
	}

	dlist_add_next(dlist_head_init(&task->task_link), &parent->children_tasks);

	return 0;

out_free_resource:
	failed = res_desc;
	task_resource_foreach(res_desc) {
		if (res_desc == failed) {
			break;
		}
		if (res_desc->deinit) {
			res_desc->deinit(task);
		}
	}
	return ret;
}

void __attribute__((noreturn)) task_exit(void *res) {
	struct task *task = task_self();
	struct thread *thread, *next;
	const struct task_resource_desc *res_desc;

	assert(critical_allows(CRITICAL_SCHED_LOCK));

	assert(task != task_kernel_task());

	task->err = (int)res;

	sched_lock();
	{
		/* Deinitialize all resources */
		task_resource_foreach(res_desc) {
			if (res_desc->deinit) {
				res_desc->deinit(task);
			}
		}

		/* Remove us from list of tasks */
		dlist_del(&task->task_link);

		/* Release our task id */
		task_table_del(task->tid);

		/*
		 * Terminate all threads except main thread. If we terminate current
		 * thread then until we in sched_lock() we continue processing
		 * and our thread structure is not freed.
		 */
		if(!dlist_empty(&task->main_thread->thread_link)) {
			dlist_foreach_entry(thread, next, &task->main_thread->thread_link, thread_link) {
				thread_terminate(thread);
			}
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

int task_set_priority(struct task *tsk, task_priority_t new_priority) {
	struct thread *t;
	task_priority_t tsk_pr;
	sched_priority_t prior;

	assert(tsk);

	if ((new_priority < TASK_PRIORITY_MIN)
			|| (new_priority > TASK_PRIORITY_MAX)) {
		return -EINVAL;
	}

	sched_lock();
	{
		if (tsk->priority == new_priority) {
			sched_unlock();
			return 0;
		}

		tsk_pr = tsk->priority;
		tsk->priority = new_priority;

		task_foreach_thread(t, tsk) {
			/* reschedule thread */
			prior = sched_priority_thread(tsk_pr, thread_priority_get(t));
			thread_set_priority(t, prior);
		}

	}
	sched_unlock();

	return 0;
}




short task_get_priority(struct task *tsk) {
	assert(tsk);
	return tsk->priority;
}

int task_table_add(struct task *task) {
	int res = util_idx_table_add((util_idx_table_t *) &task_table, task);

	if (res >= 0) {
		task->tid = res;
	}

	return res;
}

struct task *task_table_get(int n) {
	if(n < 0) {
		return NULL;
	}
	return (struct task *) util_idx_table_get((util_idx_table_t *) &task_table, n);
}

void task_table_del(int n) {
	util_idx_table_del((util_idx_table_t *) &task_table, n);
}

int task_table_has_space(void) {
	return (util_idx_table_next_alloc((util_idx_table_t *) &task_table) >= 0);
}

int task_table_get_first(int since) {
	return util_idx_table_next_mark((util_idx_table_t *) &task_table, since, 1);
}

static int task_table_init(void) {
	UTIL_IDX_TABLE_INIT(&task_table, TASK_QUANT);
	return 0;
}

int unit_init(void) {

	task_table_init();

	task_table_add(task_kernel_task());

	return 0;
}

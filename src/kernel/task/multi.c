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
#include <kernel/task/resource.h>
#include <kernel/task/resource/errno.h>

#include <err.h>

EMBOX_UNIT_INIT(multi_init);

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

int new_task(const char *name, void *(*run)(void *), void *arg) {
	struct task_creat_param *param;
	struct thread *thd = NULL;
	struct task *self_task = NULL;
	int res = 0;
	const int task_sz = sizeof *self_task + TASK_RESOURCE_SIZE;
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
			res = err(thd);
			goto out_poolfree;
		}

		addr = thread_stack_alloc(thd, task_sz);
		if (addr == NULL) {
			res = -ENOMEM;
			goto out_threadfree;
		}

		self_task = task_init(addr, task_sz, name);
		if (self_task == NULL) {
			res = -EPERM;
			goto out_threadfree;
		}

		thd->task = self_task;
		self_task->main_thread = thd;

		self_task->tsk_priority = task_self()->tsk_priority;

		/* initialize the new task */
		if ((res = task_table_add(self_task)) < 0) {
			goto out_threadfree;
		}

		res = task_resource_inherit(self_task, task_self());
		if (res != 0) {
			goto out_tablefree;
		}

		thread_set_priority(thd,
				sched_priority_thread(task_self()->tsk_priority,
						thread_priority_get(thread_self())));

		thread_detach(thd);
		thread_launch(thd);

		res = self_task->tsk_id;

		goto out_unlock;

out_tablefree:
		task_table_del(self_task->tsk_id);

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
	struct thread *th = thread_self();

	assert(th);

	return th->task;
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

void __attribute__((noreturn)) task_exit(void *res) {
	struct task *task = task_self();
	struct thread *thread, *next;

	assert(task != task_kernel_task());

	*task_resource_errno(task) = (int)res;

	sched_lock();
	{
		/* Deinitialize all resources */
		task_resource_deinit(task);

		/*
		 * Terminate all threads except main thread. If we terminate current
		 * thread then until we in sched_lock() we continue processing
		 * and our thread structure is not freed.
		 */
		dlist_foreach_entry(thread, next, &task->main_thread->thread_link, thread_link) {
			thread_terminate(thread);
		}

		/* At the end terminate main thread */
		thread_terminate(task->main_thread);

		/* Set an exited state on main thread */
		thread_state_exited(task->main_thread);

		/* Re-schedule */
		schedule();
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
		if (tsk->tsk_priority == new_priority) {
			sched_unlock();
			return 0;
		}

		tsk_pr = tsk->tsk_priority;
		tsk->tsk_priority = new_priority;

		task_foreach_thread(t, tsk) {
			/* reschedule thread */
			prior = sched_priority_thread(tsk_pr, thread_priority_get(t));
			thread_set_priority(t, prior);
		}

	}
	sched_unlock();

	return 0;
}

int multi_init(void) {
	task_table_init();

	task_table_add(task_kernel_task());

	return 0;
}

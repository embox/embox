/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <kernel/panic.h>
#include <kernel/sched.h>
#include <kernel/sched/sched_lock.h>
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/errno.h>
#include <kernel/task/task_table.h>
#include <kernel/thread.h>
#include <mem/misc/pool.h>
#include <stdint.h>
#include <string.h>
#include <util/binalign.h>

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

struct task *task_self(void) {
	struct thread *th = thread_self();

	assert(th);

	return th->task;
}

static void * task_trampoline(void *arg) {
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

int new_task(const char *name, void *(*run)(void *), void *arg) {
	struct task_creat_param *param;
	struct thread *thd = NULL;
	struct task *self_task = NULL;
	int res, tid;

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

		self_task = thread_stack_alloc(thd,
				sizeof *self_task + TASK_RESOURCE_SIZE);
		if (self_task == NULL) {
			res = -ENOMEM;
			goto out_threadfree;
		}

		tid = task_table_add(self_task);
		if (tid < 0) {
			res = tid;
			goto out_threadfree;
		}

		task_init(self_task, tid, name, thd,
				 task_self()->tsk_priority);

		res = task_resource_inherit(self_task, task_self());
		if (res != 0) {
			goto out_tablefree;
		}

		thread_set_priority(thd,
				sched_priority_thread(self_task->tsk_priority,
						thread_priority_get(thread_self())));

		thread_detach(thd);
		thread_launch(thd);

		res = self_task->tsk_id;

		goto out_unlock;

out_tablefree:
		task_table_del(tid);

out_threadfree:
		thread_terminate(thd);

out_poolfree:
		pool_free(&creat_param, param);

	}
out_unlock:
	sched_unlock();

	if (res >= 0)
		assert(self_task == task_table_get(tid));
	return res;
}

void task_init(struct task *tsk, int id, const char *name,
		struct thread *main_thread, task_priority_t priority) {
	assert(tsk != NULL);
	assert(binalign_check_bound((uintptr_t)tsk, sizeof(void *)));

	tsk->tsk_id = id;

	strncpy(tsk->tsk_name, name, sizeof tsk->tsk_name - 1);
	tsk->tsk_name[sizeof tsk->tsk_name - 1] = '\0';

	tsk->tsk_main = main_thread;
	main_thread->task = tsk;

	tsk->tsk_priority = priority;

	tsk->tsk_clock = 0;

	task_resource_init(tsk);
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
		dlist_foreach_entry(thread, next, &task->tsk_main->thread_link, thread_link) {
			thread_terminate(thread);
		}

		/* At the end terminate main thread */
		thread_terminate(task->tsk_main);

		/* Set an exited state on main thread */
		thread_state_exited(task->tsk_main);

		/* Re-schedule */
		schedule();
	}
	sched_unlock();

	/* NOTREACHED */
	panic("Returning from task_exit()");
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

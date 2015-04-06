/**
 * @file
 * @brief
 *
 * @date 31.08.11
 * @author Anton Kozlov
 */

#include <assert.h>
#include <stdint.h>
#include <string.h>
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
#include <kernel/thread/thread_priority.h>

#include <util/binalign.h>
#include <util/err.h>

struct task_trampoline_arg {
	void * (*run)(void *);
	void *run_arg;
};

struct task *task_self(void) {
	struct thread *th = thread_self();

	assert(th);
	assert(th->task);

	return th->task;
}

static void * task_trampoline(void *arg_) {
	struct task_trampoline_arg *arg = arg_;
	void *res;

	res = arg->run(arg->run_arg);
	task_exit(res);

	/* NOTREACHED */

	panic("Returning from task_trampoline()");

	return res;
}

int new_task(const char *name, void * (*run)(void *), void *arg) {
	struct task_trampoline_arg *trampoline_arg;
	struct thread *thd = NULL;
	struct task *self_task = NULL;
	int res, tid;

	sched_lock();
	{
		if (!task_table_has_space()) {
			res = -ENOMEM;
			goto out_unlock;
		}

		/*
		 * Thread does not run until we go through sched_unlock()
		 */
		thd = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED,
				task_trampoline, NULL);
		if (0 != err(thd)) {
			res = err(thd);
			goto out_unlock;
		}

		trampoline_arg = thread_stack_alloc(thd,
				sizeof *trampoline_arg);
		if (trampoline_arg == NULL) {
			res = -ENOMEM;
			goto out_threadfree;
		}

		trampoline_arg->run = run;
		trampoline_arg->run_arg = arg;
		thread_set_run_arg(thd, trampoline_arg);

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

		task_init(self_task, tid, task_self(), name, thd, task_self()->tsk_priority);

		res = task_resource_inherit(self_task, task_self());
		if (res != 0) {
			goto out_tablefree;
		}

		schedee_priority_set(&thd->schedee,
			schedee_priority_get(&thread_self()->schedee));

		thread_detach(thd);
		thread_launch(thd);

		res = self_task->tsk_id;

		goto out_unlock;

out_tablefree:
		task_table_del(tid);

out_threadfree:
		thread_terminate(thd);
	}
out_unlock:
	sched_unlock();

	return res;
}

int task_start(struct task *task, void * (*run)(void *), void *arg) {
	struct task_trampoline_arg *trampoline_arg;
	struct thread *thd = NULL;
	int res;

	sched_lock();
	{
		thd = task->tsk_main;
		trampoline_arg = thread_stack_alloc(thd, sizeof *trampoline_arg);
		if (trampoline_arg == NULL) {
			res = -ENOMEM;
			goto out_threadfree;
		}

		trampoline_arg->run = run;
		trampoline_arg->run_arg = arg;
		thread_set_run_arg(thd, trampoline_arg);

		thread_detach(thd);
		thread_launch(thd);

		res = 0;

		goto out_unlock;
out_threadfree:
		thread_terminate(thd);
	}
out_unlock:
	sched_unlock();

	return res;
}

int task_prepare(const char *name) {
	struct thread *thd = NULL;
	struct task *self_task = NULL;
	int res, tid;

	sched_lock();
	{
		if (!task_table_has_space()) {
			res = -ENOMEM;
			goto out_unlock;
		}

		/*
		 * Thread does not run until we go through sched_unlock()
		 */
		thd = thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED,
				task_trampoline, NULL);
		if (0 != err(thd)) {
			res = err(thd);
			goto out_unlock;
		}

		schedee_priority_set(&thd->schedee,
			schedee_priority_get(&thread_self()->schedee));

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

		task_init(self_task, tid, task_self(), name, thd, task_self()->tsk_priority);

		res = task_resource_inherit(self_task, task_self());
		if (res != 0) {
			goto out_tablefree;
		}

		res = self_task->tsk_id;

		goto out_unlock;

out_tablefree:
		task_table_del(tid);

out_threadfree:
		thread_terminate(thd);
	}
out_unlock:
	sched_unlock();

	return res;
}

void task_init(struct task *tsk, int id, struct task *parent, const char *name,
		struct thread *main_thread, task_priority_t priority) {
	assert(tsk != NULL);
	assert(binalign_check_bound((uintptr_t)tsk, sizeof(void *)));

	tsk->tsk_id = id;
	tsk->status = 0;

	dlist_init(&tsk->child_list);
	dlist_head_init(&tsk->child_lnk);

	task_set_name(tsk, name);

	if (main_thread) {
		tsk->tsk_main = main_thread;
		main_thread->task = tsk;
	}

	tsk->parent = parent;
	if (parent) {
		dlist_add_prev(&tsk->child_lnk, &parent->child_list);
	}

	tsk->tsk_priority = priority;

	tsk->tsk_clock = 0;

	task_resource_init(tsk);
}

void task_do_exit(struct task *task, int status) {
	struct thread *thr, *main_thr;

	assert(critical_inside(CRITICAL_SCHED_LOCK));

	main_thr = task->tsk_main;
	assert(main_thr);

	task->status = status;

	/* Deinitialize all resources */
	task_resource_deinit(task);

	/*
	 * Terminate all threads except main thread. If we terminate current
	 * thread then until we in sched_lock() we continue processing
	 * and our thread structure is not freed.
	 */
	dlist_foreach_entry(thr, &main_thr->thread_link, thread_link) {
		thread_terminate(thr);
		thread_delete(thr);
	}

	/* At the end terminate main thread */
	thread_terminate(main_thr);
}

void task_start_exit(void) {

	assert(task_self() != task_kernel_task());

	sched_lock();

	assert(critical_inside(CRITICAL_SCHED_LOCK));
}

void __attribute__((noreturn)) task_finish_exit(void) {

	assert(critical_inside(CRITICAL_SCHED_LOCK));

	/* Re-schedule */
	schedule();

	sched_unlock();

	/* NOTREACHED */
	panic("Returning from task_exit()");
}


void __attribute__((noreturn)) task_exit(void *res) {

	task_start_exit();

	task_do_exit(task_self(), TASKST_EXITED_MASK | ((int) res & TASKST_EXITST_MASK));

	task_finish_exit();
}

void __attribute__((weak)) task_mmap_deinit(const struct task *task)  {

}

void task_delete(struct task *tsk) {
	task_mmap_deinit(tsk);

	dlist_del(&tsk->child_lnk);
	task_table_del(task_get_id(tsk));
	thread_delete(task_get_main(tsk));
}

int task_set_priority(struct task *tsk, task_priority_t new_prior) {
	struct thread *t;

	assert(tsk);

	if ((new_prior < TASK_PRIORITY_MIN) || (new_prior > TASK_PRIORITY_MAX)) {
		return -EINVAL;
	}

	sched_lock();
	{
		if (tsk->tsk_priority == new_prior) {
			sched_unlock();
			return 0;
		}

		tsk->tsk_priority = new_prior;

		task_foreach_thread(t, tsk) {
			/* reschedule thread */
			schedee_priority_set(&t->schedee,
				THREAD_PRIORITY_NORMAL + new_prior);
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

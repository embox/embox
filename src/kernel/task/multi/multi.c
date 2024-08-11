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
#include <kernel/sched/schedee_priority.h>
#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource.h>
#include <kernel/task/resource/errno.h>
#include <kernel/task/task_table.h>
#include <kernel/thread.h>
#include <nsproxy.h>

#include <util/binalign.h>
#include <util/err.h>
#include <compiler.h>

#include <framework/mod/options.h>

#if OPTION_GET(NUMBER, task_quantity)
extern struct thread *main_thread_create(unsigned int flags, size_t stack_sz,
	void *(*run)(void *), void *arg);
extern void main_thread_delete(struct thread *t);
#else
#define main_thread_create thread_create_with_stack
#define main_thread_delete thread_delete
#endif /* OPTION_GET(NUMBER, task_quantity) */

struct task_trampoline_arg {
	void * (*run)(void *);
	void *run_arg;
};

struct task *task_self(void) {
	struct thread *th = thread_self();

	if (!th) {
		/* Scheduler was not started yet */
		return task_kernel_task();
	}

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

static rlim_t task_get_stack_size(struct task *parent) {
	rlim_t stack_sz;

	if (parent) {
		stack_sz = task_getrlim_stack_size(parent);
	} else {
		stack_sz = THREAD_DEFAULT_STACK_SIZE;
	}

	return stack_sz;
}

int new_task(const char *name, void * (*run)(void *), void *arg) {
	struct task_trampoline_arg *trampoline_arg;
	struct thread *thd = NULL;
	struct task *self_task = NULL;
	int res, tid;
	rlim_t stack_sz;

	/**
	 * stack_sz has effect only when task_quantity has value
	 * AND (USE_USER_STACK == 0). Otherwise stack_sz will be
	 * replaced by STACK_SZ
	 */
	stack_sz = task_get_stack_size(task_self());
	stack_sz += sizeof (struct task) + TASK_RESOURCE_SIZE;

#if OPTION_GET(NUMBER, task_quantity)
	assertf(OPTION_GET(NUMBER, resource_size) == TASK_RESOURCE_SIZE,
			"resource_size (%d) must be set up %d",
			OPTION_GET(NUMBER, resource_size), TASK_RESOURCE_SIZE);
#endif /* OPTION_GET(NUMBER, task_quantity) */

	sched_lock();
	{
		if (!task_table_has_space()) {
			res = -ENOMEM;
			goto out_unlock;
		}

		/*
		 * Thread does not run until we go through sched_unlock()
		 */
		thd = main_thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED,
				stack_sz, task_trampoline, NULL);
		if (0 != ptr2err(thd)) {
			res = ptr2err(thd);
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
	rlim_t stack_sz;

	/**
	 * stack_sz has effect only when task_quantity has value
	 * AND (USE_USER_STACK == 0). Otherwise stack_sz will be
	 * replaced by STACK_SZ
	 */
	stack_sz = task_get_stack_size(task_self());
	stack_sz += sizeof (struct task) + TASK_RESOURCE_SIZE;

	sched_lock();
	{
		if (!task_table_has_space()) {
			res = -ENOMEM;
			goto out_unlock;
		}

		/*
		 * Thread does not run until we go through sched_unlock()
		 */
		thd = main_thread_create(THREAD_FLAG_NOTASK | THREAD_FLAG_SUSPENDED,
				stack_sz, task_trampoline, NULL);
		if (0 != ptr2err(thd)) {
			res = ptr2err(thd);
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

#if defined(NET_NAMESPACE_ENABLED) && (NET_NAMESPACE_ENABLED == 1)
	set_task_proxy(tsk, parent)
#endif

	task_setrlim_stack_size(tsk, task_get_stack_size(parent));

	tsk->tsk_priority = priority;

	tsk->tsk_clock = 0;

	task_resource_init(tsk);
}

static void task_make_children_daemons(struct task *task) {
	struct task *child = NULL;
	struct task *krn_task = task_kernel_task();

	dlist_foreach_entry(child, &task->child_list, child_lnk) {
		dlist_del_init(&child->child_lnk);

		child->parent = krn_task;
		dlist_add_prev(&child->child_lnk, &krn_task->child_list);
	}
}

void task_do_exit(struct task *task, int status) {
	struct thread *thr = NULL;
	struct thread *main_thr;

	assert(critical_inside(CRITICAL_SCHED_LOCK));

	main_thr = task->tsk_main;
	assert(main_thr);

	task->status = status;

	/* Deinitialize all resources */
	task_resource_deinit(task);

	/* Make all children of the specified task daemons (or, more generally, orphans).
	 * It is made by simply setting up the parent task of each child to kernel_task. */
	task_make_children_daemons(task);

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

void _NORETURN task_finish_exit(void) {

	assert(critical_inside(CRITICAL_SCHED_LOCK));

	/* Re-schedule */
	schedule();

	sched_unlock();

	/* NOTREACHED */
	panic("Returning from task_exit()");
}


void _NORETURN task_exit(void *res) {

	task_start_exit();

	task_do_exit(task_self(), TASKST_EXITED_MASK | ((intptr_t) res & TASKST_EXITST_MASK));

	task_finish_exit();
}

void task_delete(struct task *tsk) {
	netns_decrement_ref_cnt(tsk->nsproxy.net_ns);
	dlist_del(&tsk->child_lnk);
	task_table_del(task_get_id(tsk));
	main_thread_delete(task_get_main(tsk));
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
				SCHED_PRIORITY_NORMAL + new_prior);
		}

	}
	sched_unlock();

	return 0;
}

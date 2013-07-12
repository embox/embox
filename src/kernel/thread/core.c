/**
 * @file
 * @brief Thread management API implementation.
 *
 * @date 22.04.10
 * @author Dmitry Avdyukhin
 *          - Initial implementation
 * @author Alina Kramar
 *          - Thread control block memory allocation
 *          - Move state management code into the scheduler
 *          - Suspend/resume logic
 * @author Eldar Abusalimov
 *          - Reviewing and simplifying threads API
 *          - Stack allocation
 * @author Anton Kozlov
 *          - Tasks binding
 *
 * @see tests/kernel/thread/core_test.c
 */

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>

#include <err.h>

#include <embox/unit.h>

#include <util/math.h> /*clamp */
#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/sched.h>
#include <kernel/thread/state.h>
#include <kernel/thread/wait_data.h>

#include <kernel/panic.h>

#include <hal/context.h>
#include <hal/arch.h> /*only for arch_idle */

#include <kernel/cpu.h>
#include <kernel/percpu.h>

EMBOX_UNIT_INIT(thread_core_init);

//struct list_head __thread_list = LIST_HEAD_INIT(__thread_list);
DLIST_DEFINE(__thread_list);

static int id_counter;

static void thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg);
static void thread_context_init(struct thread *t);

static struct thread *thread_new(void);
static void thread_delete(struct thread *t);

extern struct thread *thread_alloc(void);
extern void thread_free(struct thread *t);

/**
 * Wrapper for thread start routine.
 */
static void __attribute__((noreturn)) thread_trampoline(void) {
	struct thread *current = thread_self();
	void *res;

	assert(!critical_allows(CRITICAL_SCHED_LOCK));

	sched_unlock_noswitch();
	ipl_enable();

	assert(!critical_inside(CRITICAL_SCHED_LOCK));

	res = current->run(current->run_arg);
	thread_exit(res);
}

int thread_create(struct thread **p_thread, unsigned int flags,
		void *(*run)(void *), void *arg) {
	struct thread *t;
	int save_ptr;
	int res = ENOERR;

	/* check mutually exclusive flags */
	if ((flags & THREAD_FLAG_PRIORITY_LOWER)
			&& (flags & THREAD_FLAG_PRIORITY_HIGHER)) {
		return -EINVAL;
	}

	/* check one more mutually exclusive flags */
	if ((flags & THREAD_FLAG_SUSPENDED) && (flags & THREAD_FLAG_DETACHED)) {
		return -EINVAL;
	}

	/* check whether we need to return thread structure pointer */
	save_ptr = (flags & THREAD_FLAG_SUSPENDED)
			|| !(flags & THREAD_FLAG_DETACHED);

	/* if we need thread handler we check place for result */
	if (save_ptr && !p_thread) {
		return -EINVAL;
	}

	/* check correct executive function */
	if (!run) {
		return -EINVAL;
	}

	sched_lock(); /* lock scheduling */

		if (!(t = thread_new())) {
			res = -ENOMEM;
			goto out;
		}

		thread_init(t, flags, run, arg);
		thread_context_init(t);

		if (!(flags & THREAD_FLAG_SUSPENDED)) {
			thread_launch(t);
		}

		if (flags & THREAD_FLAG_DETACHED) {
			thread_detach(t);
		}

out:
	sched_unlock();

	if (save_ptr) {
		*p_thread = t; /* save result pointer */
	}

	return res;
}

static void thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg) {
	struct task *tsk;
	__thread_priority_t priority;

	assert(t);
	assert(run);

	/* get task pointer in depends on whether thread create for kernel task
	 * or user task */
	tsk = (flags & THREAD_FLAG_KTASK) ? task_kernel_task() : task_self();

	if (tsk) { //TODO may be if tsk == NULL it's an error
		t->task = tsk;
		list_add(&t->task_link, &tsk->threads);
	}

	t->state = thread_state_init();

	/* set executive function and arguments pointer */
	t->run = run;
	t->run_arg = arg;

	/* calculate current thread priority. It can be change later with
	 * thread_set_priority () function
	 */
	if (flags & THREAD_FLAG_PRIORITY_INHERIT) {
		priority = thread_self()->priority;
	} else {
		priority = THREAD_PRIORITY_DEFAULT;
	}

	if ((flags & THREAD_FLAG_PRIORITY_LOWER)
			&& (priority >= THREAD_PRIORITY_MIN)) {
		priority--;
	} else if ((flags & THREAD_FLAG_PRIORITY_HIGHER)
			&& (priority <= THREAD_PRIORITY_HIGH)) {
		priority++;
	}

	t->priority = priority; //clamp(t->priority, THREAD_PRIORITY_MIN, THREAD_PRIORITY_HIGH);

	t->initial_priority = get_sched_priority(t->task->priority, t->priority);
	t->sched_priority = t->initial_priority;

	sched_strategy_init(&t->sched);

	t->joined = NULL;

	t->running_time = 0;
	t->affinity = (1 << NCPU) - 1;

	wait_data_init(&t->wait_data);
}

static void thread_context_init(struct thread *t) {
	assert(t);
	assert(t->stack);
	assert(t->stack_sz);

	context_init(&t->context, true);
	context_set_entry(&t->context, thread_trampoline);
	context_set_stack(&t->context, (char *) t->stack + t->stack_sz);
}

void __attribute__((noreturn)) thread_exit(void *ret) {
	struct thread *current = thread_self();
	struct task *task = task_self();
	struct thread *thread;
	int count = 0;

	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		/* Counting number of threads in task. XXX: not the best way */
		list_for_each_entry(thread, &task->threads, task_link) {
			count++;
		}

		if ((count == 1) ||
			(count == 2 && thread_state_dead(task->main_thread->state))) {

			/* We are last thread. Unlock scheduler and exit task. */
			sched_unlock();
			task_exit(ret);
		} else {
			/* Finish scheduling of the thread */
			sched_finish(current);

			/* We can free only not main threads */
			if (current != task->main_thread) {
				if (thread_state_dead(current->state)) {
					/* Thread is detached. Should be deleted by itself. */
					thread_delete(current);
				} else {
					/* Thread is attached. Joined thread delete it.    */
					current->run_ret = ret;

					if (current->joined) {
						sched_thread_notify(current->joined, ENOERR);
					}
				}
			}
		}
	}
	sched_unlock();

	/* NOTREACHED */
	panic("Returning from thread_exit()");
}

int thread_join(struct thread *t, void **p_ret) {
	struct thread *current = thread_self();
	void *join_ret;

	assert(t);

	if (t == current) {
		return -EDEADLK;
	}

	sched_lock();
	{
		if (!thread_state_exited(t->state)) {
			/* Target thread is not exited. Waiting for his exiting. */
			assert(!t->joined);
			t->joined = current;

			sched_prepare_wait(NULL, NULL);
			sched_wait_locked(SCHED_TIMEOUT_INFINITE);
			sched_cleanup_wait();
		}

		join_ret = t->run_ret;
		t->state = thread_state_do_detach(t->state);
		thread_delete(t);

		if (p_ret) {
			*p_ret = join_ret;
		}
	}
	sched_unlock();

	return 0;
}

int thread_detach(struct thread *t) {
	assert(t);

	sched_lock();
	{
		t->state = thread_state_do_detach(t->state);

		if (thread_state_dead(t->state)) {
			/* The target thread has finished, free it here. */
			thread_delete(t);
		}
	}
	sched_unlock();

	return 0;
}

int thread_launch(struct thread *t) {
	assert(t);

	sched_lock();
	{
		if (thread_state_started(t->state)) {
			sched_unlock();
			return -EINVAL;
		}

		if (thread_state_exited(t->state)) {
			sched_unlock();
			return -ESRCH;
		}

		sched_start(t);
	}
	sched_unlock();

	return 0;
}

int thread_terminate(struct thread *t) {
	assert(t);

	sched_lock();
	{
		if (!thread_state_exited(t->state)) {
			sched_finish(t);
		}

		if (!thread_state_detached(t->state)) {
			t->state = thread_state_do_detach(t->state);
		}

		thread_delete(t);
	}
	sched_unlock();

	return 0;
}

void thread_yield(void) {
	sched_post_switch();
}

int thread_set_priority(struct thread *t, thread_priority_t new_priority) {
	assert(t);

	if ((new_priority < THREAD_PRIORITY_MIN)
			|| (new_priority > THREAD_PRIORITY_MAX)) {
		return -EINVAL;
	}

	sched_lock();
	{
		if (t->priority == new_priority) {
			sched_unlock();
			return 0;
		}
		sched_set_priority(t, get_sched_priority(t->task->priority,
					new_priority));
		t->priority = new_priority;
	}
	sched_unlock();

	return 0;
}

thread_priority_t thread_get_priority(struct thread *t) {
	assert(t);

	return t->priority;
}

clock_t thread_get_running_time(struct thread *thread) {
	sched_lock();
	{
		if (thread_state_oncpu(thread->state)) {
			/* Recalculate time of the thread. */
			clock_t	new_clock = clock();
			thread->running_time += new_clock - thread->last_sync;
			thread->last_sync = new_clock;
		}
	}
	sched_unlock();

	return thread->running_time;
}


static struct thread *thread_new(void) {
	struct thread *t;

	if (!(t = thread_alloc())) {
		return NULL;
	}

	t->id = id_counter++;
	dlist_add_next(&t->thread_link, &__thread_list);

	return t;
}

static void thread_delete(struct thread *t) {
	static struct thread *zombie;
	struct thread *current = thread_self();

	assert(t);
	assert(thread_state_dead(t->state));
	assert(t != zombie);

	if (zombie != NULL && zombie != current) {
		thread_free(zombie);
		zombie = NULL;
	}

	list_del(&t->task_link);
	dlist_del(&t->thread_link);

	if (t == current) {
		zombie = t;
	} else {
		thread_free(t);
	}
}


struct thread *thread_lookup(thread_id_t id) {
	struct thread *t, *tmp;

	thread_foreach(t, tmp) {
		if (t->id == id) {
			return t;
		}
	}

	return NULL;
}

/*
 * Function, which does nothing. For idle_thread.
 */
static void *idle_run(void *arg) {
	while (true) {
		arch_idle();
	}
	return NULL;
}


struct thread *thread_idle_init(void) {
	struct thread *idle;

	if (!(idle = thread_new())) {
		return NULL;
	}
	thread_init(idle, THREAD_FLAG_KTASK, idle_run, NULL);
	thread_context_init(idle);

	idle->priority = THREAD_PRIORITY_MIN;
	idle->sched_priority = SCHED_PRIORITY_MIN;

	cpu_set_idle_thread(idle);

	return idle;
}

struct thread *thread_init_self(void *stack, size_t stack_sz,
		thread_priority_t priority) {
	struct thread *thread = stack; /* Allocating at the bottom */

	/* Stack setting up */
	thread->stack = stack + sizeof(struct thread);
	thread->stack_sz = stack_sz - sizeof(struct thread);

	/* Global list addition and id setting up */
	thread->id = id_counter++;
	dlist_add_next(&thread->thread_link, &__thread_list);

	/* General initialization and task setting up */
	thread_init(thread, THREAD_FLAG_KTASK, idle_run, NULL);

	/* Priority setting up */
	thread->priority = priority;
	thread->sched_priority = get_sched_priority(thread->task->priority,
			thread->priority);

	return thread;
}

struct thread *thread_boot_init(void) {
	struct thread *bootstrap;
	struct task *kernel_task = task_kernel_task();
	extern char _stack_vma, _stack_len;

	id_counter = 0;

	/* TODO: priority */
	bootstrap = thread_init_self(&_stack_vma, (uint32_t) &_stack_len,
			THREAD_PRIORITY_NORMAL);

	kernel_task->main_thread = bootstrap;

	return bootstrap;
}

extern int sched_init(struct thread *idle, struct thread *current);

static int thread_core_init(void) {
	struct thread *idle;
	struct thread *current;

	idle = thread_idle_init();
	current = thread_boot_init();
	return sched_init(idle, current);
}

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

#include <embox/unit.h>

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

DLIST_DEFINE(__thread_list);

static int id_counter;

static void thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg);
//static void thread_context_init(struct thread *t);

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

	/* below we will work with thread's instances and therefore we need to
	 * lock scheduler (disable scheduling) to our structures is not be
	 * corrupted
	 */
	sched_lock(); /* lock scheduling */

		/*allocate memory, setup thread_id and insert to global thread's list*/
		if (!(t = thread_new())) {
			res = -ENOMEM;
			goto out;
		}

		/* initialize internal thread structure */
		thread_init(t, flags, run, arg);


		/* link with task if it need */
		if(!(flags & THREAD_FLAG_KTASK)) {
			task_add_thread(task_self(), t);
		}

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
	__thread_priority_t priority;

	assert(t);
	assert(run);
	assert(t->stack);
	assert(t->stack_sz);


	dlist_init(&t->task_link); /* default unlink value */

	t->state = thread_state_init();

	/* set executive function and arguments pointer */
	t->run = run;
	t->run_arg = arg;

	/* calculate current thread priority. It can be change later with
	 * thread_set_priority () function
	 */
	if (flags & THREAD_FLAG_PRIORITY_INHERIT) {
		priority = thread_priority_get(thread_self());
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

	/* setup thread priority. Now we have not started thread yet therefore we
	 * just set both base and scheduling priority in default value.
	 */
	thread_priority_set(t, priority);

	t->joined = NULL; /* there are not any joined thread */

	/* cpu context init */
	context_init(&t->context, true); /* setup default value of CPU registers */
	context_set_entry(&t->context, thread_trampoline); /* set entry (IP register */
	context_set_stack(&t->context, (char *) t->stack + t->stack_sz); /* set SP */


	sched_strategy_init(&t->sched);

	t->affinity = (1 << NCPU) - 1;

	wait_data_init(&t->wait_data);
}

void __attribute__((noreturn)) thread_exit(void *ret) {
	struct thread *current = thread_self();
	struct task *task = task_self();

	assert(critical_allows(CRITICAL_SCHED_LOCK));

	sched_lock();
	{
		/* We can free only not main threads */
		if(task->main_thread == current) {
			/* We are last thread. Unlock scheduler and exit task. */
			sched_unlock();
			task_exit(ret);
		} else {
			/* Finish scheduling of the thread */
			sched_finish(current);

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

		/* running time */
		t->running_time = 0;
		t->last_sync = clock();

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

	thread_priority_set(t, new_priority);

	return 0;
}
#if 0
thread_priority_t thread_get_priority(struct thread *t) {
	assert(t);

	return t->priority;
}
#endif

clock_t thread_get_running_time(struct thread *thread) {
	clock_t new_clock;

	sched_lock();
	{
		if (thread_state_oncpu(thread->state)) {
			/* Recalculate time of the thread. */

			new_clock = clock();
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

	dlist_head_init(&t->thread_link);
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

	task_remove_thread(t->task, t);
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
	idle->task = task_kernel_task();
	idle->task->main_thread = idle;

	thread_priority_set(idle, THREAD_PRIORITY_MIN);

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

	dlist_head_init(&thread->thread_link);
	dlist_add_next(&thread->thread_link, &__thread_list);

	/* General initialization and task setting up */
	thread_init(thread, 0, idle_run, NULL);

	/* Priority setting up */
	thread_priority_set(thread, priority);

	/* running time */
	thread->running_time = clock();
	thread->last_sync = thread->running_time;

	return thread;
}

struct thread *thread_boot_init(void) {
	struct thread *bootstrap;
	struct task *kernel_task = task_kernel_task();
	extern char _stack_vma, _stack_len;

	/* TODO: priority */
	bootstrap = thread_init_self(&_stack_vma, (uint32_t) &_stack_len,
			THREAD_PRIORITY_NORMAL);

	task_add_thread(kernel_task, bootstrap);

	return bootstrap;
}

extern int sched_init(struct thread *idle, struct thread *current);

static int thread_core_init(void) {
	struct thread *idle;
	struct thread *current;
	struct task *kernel_task = task_kernel_task();


	id_counter = 0; /* start enumeration */

	idle = thread_idle_init(); /* idle thread always has ID=0 */
	kernel_task->main_thread = idle;

	current = thread_boot_init(); /* 'init' thread ID=1 */

	return sched_init(idle, current);
}

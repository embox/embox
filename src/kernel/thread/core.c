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
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <inttypes.h>

#include <embox/unit.h>

#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/sched.h>
#include <kernel/thread/signal.h>
#include <kernel/thread/thread_alloc.h>
#include <kernel/thread/thread_local.h>
#include <kernel/thread/thread_sched_wait.h>
#include <kernel/sched/schedee_priority.h>
#include <kernel/sched/current.h>
#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>
#include <kernel/cpu/cpudata.h>

#include <kernel/panic.h>

#include <hal/context.h>
#include <util/err.h>
#include <compiler.h>

extern void thread_context_switch(struct thread *prev, struct thread *next);
extern void thread_ack_switched(void);

static int id_counter = 1; // TODO make it an indexator

static struct thread *__current_thread __cpudata__;

/* Used in boot_thread.c and thread_switch.c */
void thread_set_current(struct thread *t) {
	cpudata_var(__current_thread) = t;
}

struct thread *thread_self(void) {
	return cpudata_var(__current_thread);
}

/**
 * Wrapper for thread start routine.
 * Called from sched_switch() function with interrupts off.
 */
static void _NORETURN thread_trampoline(void) {
	struct thread *current = thread_self();
	void *res;

	assertf(!critical_allows(CRITICAL_SCHED_LOCK), "0x%" PRIx32 "", (uint32_t)__critical_count);

	thread_ack_switched();

	assert(!critical_inside(CRITICAL_SCHED_LOCK));

	/* execute user function handler */
	res = current->run(current->run_arg);
	thread_exit(res);
	/* NOTREACHED */
}

int thread_priority_by_flags(unsigned int flags) {
	int priority;

	if (flags & THREAD_FLAG_PRIORITY_INHERIT) {
		priority = schedee_priority_get(&thread_self()->schedee);
	} else {
		priority = SCHED_PRIORITY_NORMAL;
	}

	if ((flags & THREAD_FLAG_PRIORITY_LOWER)
			&& (priority > SCHED_PRIORITY_MIN)) {
		priority--;
	} else if ((flags & THREAD_FLAG_PRIORITY_HIGHER)
			&& (priority < SCHED_PRIORITY_HIGH)) {
		priority++;
	}

	return priority;
}

static struct thread *__thread_create(unsigned int flags, size_t stack_sz,
	    void *(*run)(void *), void *arg) {
	struct thread *t;
	int priority;

	/* check mutually exclusive flags */
	if ((flags & THREAD_FLAG_PRIORITY_LOWER)
			&& (flags & THREAD_FLAG_PRIORITY_HIGHER)) {
		return err_ptr(EINVAL);
	}

	if((flags & THREAD_FLAG_NOTASK) && !(flags & THREAD_FLAG_SUSPENDED)) {
		return err_ptr(EINVAL);
	}

	/* check correct executive function */
	if (!run) {
		return err_ptr(EINVAL);
	}

	/* calculate current thread priority. It can be change later with
	 * thread_set_priority () function
	 */
	priority = thread_priority_by_flags(flags);

	/* below we will work with thread's instances and therefore we need to
	 * lock scheduler (disable scheduling) to our structures is not be
	 * corrupted
	 */
	sched_lock();
	{
		/* allocate memory */
		if (!(t = thread_alloc(stack_sz))) {
			t = err_ptr(ENOMEM);
			goto out_unlock;
		}

		/* initialize internal thread structure */
		thread_init(t, priority, run, arg);

		/* link with task if needed */
		if (!(flags & THREAD_FLAG_NOTASK)) {
			task_thread_register(task_self(), t);
		}

		thread_cancel_init(t);

		if (!(flags & THREAD_FLAG_SUSPENDED)) {
			thread_launch(t);
		}

		if (flags & THREAD_FLAG_DETACHED) {
			thread_detach(t);
		}

	}
out_unlock:
	sched_unlock();

	return t;
}

struct thread *thread_create(unsigned int flags,
		void *(*run)(void *), void *arg) {
	rlim_t stack_sz;

	if (flags & THREAD_FLAG_NOTASK) {
		stack_sz = THREAD_DEFAULT_STACK_SIZE;
	} else {
		stack_sz = task_getrlim_stack_size(task_self());
	}

	return __thread_create(flags, stack_sz, run, arg);
}

struct thread *thread_create_with_stack(unsigned int flags,
		size_t stack_sz, void *(*run)(void *), void *arg) {
	return __thread_create(flags, stack_sz, run, arg);
}

static struct schedee *thread_process(struct schedee *prev, struct schedee *next) {
	struct thread *next_t, *prev_t;

	next_t = mcast_out(next, struct thread, schedee);
	prev_t = mcast_out(prev, struct thread, schedee);


	/* Threads context switch */
	if (prev != next) {
		thread_context_switch(prev_t, next_t);
	}

	ipl_enable();

	if (!prev_t->siglock) {
		thread_signal_handle();
	}

	return &thread_self()->schedee;
}

void thread_init(struct thread *t, int priority,
		void *(*run)(void *), void *arg) {

	assert(t);
	assert(run);
	assert(thread_stack_get(t));
	assert(thread_stack_get_size(t));

	t->id = id_counter++; /* setup thread ID */

	dlist_head_init(&t->thread_link); /* default unlink value */

	t->task = NULL;

	t->critical_count = __CRITICAL_COUNT(CRITICAL_SCHED_LOCK);
	t->siglock = 0;

	t->state = TS_INIT;

	if (thread_local_alloc(t, MODOPS_THREAD_KEY_QUANTITY)) {
		panic("can't initialize thread_local");
	}

	t->joining = NULL;

	t->run = run;
	t->run_arg = arg;

	/* cpu context init */
	/* setup stack pointer to the top of allocated memory
	 * The structure of kernel thread stack follow:
	 * +++++++++++++++ top
	 *                  |
	 *                  v
	 * the thread structure
	 * xxxxxxx
	 * the end
	 * +++++++++++++++ bottom (t->stack - allocated memory for the stack)
	 */
#ifndef CONTEXT_USE_STACK_SIZE
	context_init(&t->context, CONTEXT_PRIVELEGED | CONTEXT_IRQDISABLE,
			thread_trampoline, thread_stack_get(t) + thread_stack_get_size(t));
#else
	context_init(&t->context, CONTEXT_PRIVELEGED | CONTEXT_IRQDISABLE,
			thread_trampoline, thread_stack_get(t) + thread_stack_get_size(t),
			thread_stack_get_size(t));
#endif

	sigstate_init(&t->sigstate);

	schedee_init(&t->schedee, priority, thread_process, SCHEDEE_THREAD);

	/* initialize everthing else */
	thread_wait_init(&t->thread_wait_list);
}

struct thread *thread_init_stack(void *stack, size_t stack_sz,
	       	int priority, void *(*run)(void *), void *arg) {
	struct thread *thread = stack; /* Allocating at the bottom */

	/* Stack setting up */
	thread_stack_init(thread, stack_sz);

	/* General initialization and task setting up */
	thread_init(thread, priority, run, arg);

	return thread;

}
void thread_delete(struct thread *t) {
	static struct thread *zombie = NULL;

	assert(t);
	assert(t->state & TS_EXITED);

	task_thread_unregister(t->task, t);
	thread_local_free(t);
	thread_wait_deinit(&t->thread_wait_list);

	if (zombie) {
		thread_free(zombie);
	}

	if (t != thread_self()) {
		assert(!t->schedee.active);
		assert(!t->schedee.ready);
		thread_free(t);
		zombie = NULL;
	} else {
		zombie = t;
	}
}

void _NORETURN thread_exit(void *ret) {
	struct thread *current = thread_self();
	struct task *task = task_self();
	struct thread *joining;

	/* We can free only not main threads */
	if (current == task_get_main(task)) {
		/* We are last thread. */
		task_exit(ret);
		/* NOTREACHED */
	}

	sched_lock();

	// sched_finish(current);
	current->schedee.waiting = true;
	current->state |= TS_EXITED;

	/* Wake up a joining thread (if any).
	 * Note that joining and run_ret are both in a union. */
	joining = current->joining;
	current->run_ret = ret;
	if (joining) {
		sched_wakeup(&joining->schedee);
	}

	if (current->state & TS_DETACHED)
		/* No one references this thread anymore. Time to delete it. */
		thread_delete(current);

	schedule();

	/* NOTREACHED */
	sched_unlock();  /* just to be honest */
	panic("Returning from thread_exit()");
}

int thread_join(struct thread *t, void **p_ret) {
	struct thread *current = thread_self();
	int ret = 0;

	assert(t);

	if (t == current)
		return -EDEADLK;

	sched_lock();
	{
		assert(!(t->state & TS_DETACHED));

		if (!(t->state & TS_EXITED)) {
			assert(!t->joining);
			t->joining = current;

			ret = SCHED_WAIT(t->state & TS_EXITED);
			if (ret) {
				goto out;
			}
		}

		if (p_ret)
			*p_ret = t->run_ret;

		thread_delete(t);
	}
out:
	sched_unlock();

	return ret < 0 ? ret : 0;
}

int thread_detach(struct thread *t) {
	assert(t);

	sched_lock();
	{
		assert(!(t->state & TS_DETACHED));

		if (!(t->state & TS_EXITED)) {
			/* The target will free itself upon finishing. */
			assert(!t->joining);
			t->state |= TS_DETACHED;
		}
		else
			/* The target thread has finished, free it here. */
			thread_delete(t);
	}
	sched_unlock();

	return 0;
}

int thread_launch(struct thread *t) {
	int ret;

	sched_lock();
	{
		if (t->state & TS_EXITED) {
			ret = -EINVAL;
		}
		else {
			ret = sched_wakeup(&t->schedee) ? 0 : -EINVAL;
		}
	}
	sched_unlock();

	return ret;
}

int thread_terminate(struct thread *t) {
	assert(t);

	sched_lock();
	{
		// sched_finish(t);
		// assert(0, "NIY");
		// thread_delete(t);
		sched_freeze(&t->schedee);

		t->state |= TS_EXITED;

		// XXX prevent scheduler to add thread in runq
		if (t == thread_self()) {
			t->schedee.waiting = true;
		}
	}
	sched_unlock();

	return 0;
}

void thread_yield(void) {
	sched_post_switch();
}

clock_t thread_get_running_time(struct thread *t) {
	clock_t running;

	sched_lock();
	{
		running = sched_timing_get(&t->schedee);
	}
	sched_unlock();

	return running;
}

void thread_set_run_arg(struct thread *t, void *run_arg) {
	assert(t->state == TS_INIT);
	t->run_arg = run_arg;
}

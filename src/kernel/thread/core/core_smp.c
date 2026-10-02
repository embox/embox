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

#include <kernel/printk.h>
#include <lib/libds/array.h>
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
#include <hal/ipl.h>
#include <kernel/cpu/cpu.h>
#include <kernel/cpu/cpudata.h>

#include <kernel/panic.h>

#include <hal/context.h>
#include <util/err.h>
#include <compiler.h>
#include <util/atomic_rmw.h>

extern void thread_context_switch(struct thread *prev, struct thread *next);
extern void thread_ack_switched(void);

static int id_counter = 1; // TODO make it an indexator

static struct thread *__current_thread __cpudata__;

/* Used in boot_thread.c and thread_switch.c */
void thread_set_current(struct thread *t) {
	cpudata_var(__current_thread) = t;
}

struct thread *thread_self(void) {
	struct thread *t;
	ipl_t ipl;

	/* The answer is "the thread running on this CPU", and this thread is it --
	 * but only for as long as it stays on this CPU. Migrate between the CPU id
	 * and the load and the answer is somebody else's thread, which is how
	 * threadsig_lock() and threadsig_unlock() came to increment and decrement
	 * two different counters. The pointer stays correct after the mask is
	 * dropped: it names this thread, not this CPU. */
	ipl = ipl_save();
	t = cpudata_var(__current_thread);
	ipl_restore(ipl);

	return t;
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
		priority = SCHED_OTHER_PRIORITY_NORM;
	}

	if ((flags & THREAD_FLAG_PRIORITY_LOWER)
			&& (priority > SCHED_PRIORITY_MIN)) {
		priority--;
	} else if ((flags & THREAD_FLAG_PRIORITY_HIGHER)
			&& (priority < SCHED_RT_PRIORITY_MAX)) { /* not for lthread */
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
		return err2ptr(EINVAL);
	}

	if((flags & THREAD_FLAG_NOTASK) && !(flags & THREAD_FLAG_SUSPENDED)) {
		return err2ptr(EINVAL);
	}

	/* check correct executive function */
	if (!run) {
		return err2ptr(EINVAL);
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
			t = err2ptr(ENOMEM);
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
	size_t stack_sz;

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

	t->magic = THREAD_MAGIC_LIVE;
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
/* Threads that have exited but whose CPU has not finished leaving them. A
 * thread cannot free its own stack, and until __sched_deactivate() sets
 * schedee.released it is still running on it. Every caller holds sched_lock(),
 * so the list needs no lock. */
static DLIST_DEFINE(thread_zombies);
static int parked;

/* The last few threads handed back to the pool, so that a use-after-free can
 * name who freed it rather than only where it was noticed. Written under
 * sched_lock(), read only when something has already gone wrong. */
struct thread_free_note {
	struct thread *t;
	void *from;
	unsigned int seq;
};
static struct thread_free_note thread_free_ring[16];
static unsigned int thread_free_seq;

static void thread_note_free(struct thread *t, void *from) {
	struct thread_free_note *n;

	t->magic = THREAD_MAGIC_DEAD;

	n = &thread_free_ring[thread_free_seq % ARRAY_SIZE(thread_free_ring)];
	n->t = t;
	n->from = from;
	n->seq = ++thread_free_seq;
}

/* Answers with a printk rather than an assertion: the caller is about to do
 * something to a thread that is not one, and what it does next is less
 * interesting than where the pointer came from. */
static int thread_is_live(struct thread *t, const char *what) {
	unsigned int i;

	if (t->magic == THREAD_MAGIC_LIVE) {
		return 1;
	}

	printk("%s(%p): not a live thread -- magic %#x, state %#x\n", what, t,
	    t->magic, t->state);

	for (i = 0; i < ARRAY_SIZE(thread_free_ring); i++) {
		if (thread_free_ring[i].t == t) {
			printk("    freed by %p, %u free(s) ago\n",
			    thread_free_ring[i].from,
			    thread_free_seq - thread_free_ring[i].seq);
		}
	}

	return 0;
}

void thread_delete(struct thread *t) {
	struct thread *z;

	assert(t);
	if (!thread_is_live(t, "thread_delete")) {
		return;
	}
	/* Name the caller. An assertion here says only that somebody
	 * deleted a live thread, which is not something anybody can act on. */
	if (!(t->state & TS_EXITED)) {
		printk("thread_delete(%p) state %#x task %p self %p, from %p\n", t,
		    t->state, t->task, thread_self(), __builtin_return_address(0));
	}
	assert(t->state & TS_EXITED);

	/* Whoever gets here first reclaims the thread; the rest only find out.
	 *
	 * A thread can be reached by two reclaimers at once: task_do_exit() walks
	 * the task's threads and deletes each one, and a sibling in thread_join()
	 * wakes the moment thread_terminate() sets TS_EXITED and deletes the very
	 * same thread. On one core the join never got to run -- the joining thread
	 * was terminated first. With the boot-core fence lifted it runs on another
	 * core, and the two deletions raced: a double sysfree() of the thread's
	 * local storage, every run of the same binary. */
	if (t->state & TS_DELETED) {
		return;
	}
	t->state |= TS_DELETED;

	task_thread_unregister(t->task, t);
	thread_local_free(t);
	thread_wait_deinit(&t->thread_wait_list);

	/* Whatever was parked earlier and has since been let go. */
	dlist_foreach_entry(z, &thread_zombies, thread_link) {
		if (atomic_rmw_load(&z->schedee.released, __ATOMIC_ACQUIRE)) {
			dlist_del(&z->thread_link);
			thread_note_free(z, __builtin_return_address(0));
			thread_free(z);
			parked--;
		}
	}
	/* A parked thread is freed by the next deletion after its core lets go of
	 * it, so the list is short by construction. If it is not, the flag is not
	 * arriving and this has turned into a leak -- say so here rather than run
	 * the pool dry and fail somewhere else. */
	assertf(parked < 32, "thread_zombies holds %d threads", parked);

	if ((t == thread_self())
	    || !atomic_rmw_load(&t->schedee.released, __ATOMIC_ACQUIRE)) {
		/* Still standing on it: this thread's own stack, or another core
		 * that has not come out of the scheduler with it yet. */
		dlist_head_init(&t->thread_link);
		dlist_add_prev(&t->thread_link, &thread_zombies);
		parked++;
	}
	else {
		assert(!t->schedee.active);
		assert(!t->schedee.ready);
		thread_note_free(t, __builtin_return_address(0));
		thread_free(t);
	}
}

/**
 * Say we have thread A and thread B and
 * thread B is the one which is going to exit and delete
 * Case 1: thread A call thread_join() before thread B exit:
 *     Thread A set the joining filed of thread B. When thread B
 *     exit after, it will give control back to thread A again.
 *     In other word, thread B is deleted by thread A in this case
 * Case 2: thread B call thread_exit first.
 *     In this case, no blocks happens
 *     If thread B is detached, it will self-deleted.
 *     If thread B is joinable, it just set some flags and ret-value,
 *     thread B deleted when thread A or other thread call thread_join()
 */
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

	/* Before `waiting`, so that a wakeup racing this one either finds a thread
	 * that is simply running (and does nothing) or finds the flag. In between
	 * there is nothing to find. */
	sched_finished(&current->schedee);

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
		if (!thread_is_live(t, "thread_join")) {
			sched_unlock();
			return -ESRCH;
		}
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
		if (!thread_is_live(t, "thread_detach")) {
			sched_unlock();
			return -ESRCH;
		}
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
		/* sched_freeze() clears `waiting`, which on its own is what the
		 * comment below calls "prevent scheduler to add thread in runq". On
		 * four cores that is not enough: a wakeup already in flight can still
		 * reach this schedee. */
		sched_finished(&t->schedee);

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

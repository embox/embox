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

#include <framework/mod/options.h>

#if OPTION_GET(NUMBER, task_quantity)

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
#include <util/atomic_rmw.h>

extern struct thread *main_thread_alloc(void);
extern void main_thread_free(struct thread *t);

struct thread *main_thread_create(unsigned int flags, size_t stack_sz,
                                  void *(*run)(void *), void *arg) {
	struct thread *t;
	int priority;

	(void) stack_sz;

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
		if (!(t = main_thread_alloc())) {
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

/* Threads that have exited but whose CPU has not finished leaving them. A
 * thread cannot free its own stack, and until __sched_deactivate() sets
 * schedee.released it is still running on it. Every caller holds sched_lock(),
 * so the list needs no lock. */
static DLIST_DEFINE(main_thread_zombies);
static int parked;

void main_thread_delete(struct thread *t) {
	struct thread *z;

	assert(t);
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
	dlist_foreach_entry(z, &main_thread_zombies, thread_link) {
		if (atomic_rmw_load(&z->schedee.released, __ATOMIC_ACQUIRE)) {
			dlist_del(&z->thread_link);
			main_thread_free(z);
			parked--;
		}
	}
	/* A parked thread is freed by the next deletion after its core lets go of
	 * it, so the list is short by construction. If it is not, the flag is not
	 * arriving and this has turned into a leak -- say so here rather than run
	 * the pool dry and fail somewhere else. */
	assertf(parked < 32, "main_thread_zombies holds %d threads", parked);

	if ((t == thread_self())
	    || !atomic_rmw_load(&t->schedee.released, __ATOMIC_ACQUIRE)) {
		/* Still standing on it: this thread's own stack, or another core
		 * that has not come out of the scheduler with it yet. */
		dlist_head_init(&t->thread_link);
		dlist_add_prev(&t->thread_link, &main_thread_zombies);
		parked++;
	}
	else {
		assert(!t->schedee.active);
		assert(!t->schedee.ready);
		main_thread_free(t);
	}
}

#endif /* OPTION_GET(NUMBER, task_quantity) */


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

#include <embox/unit.h>

#include <kernel/thread.h>
#include <kernel/task.h>
#include <kernel/sched.h>
#include <kernel/thread/signal.h>
#include <kernel/thread/thread_alloc.h>
#include <kernel/thread/thread_local.h>
#include <kernel/thread/thread_register.h>
#include <kernel/sched/sched_priority.h>

#include <kernel/panic.h>

#include <hal/context.h>
#include <err.h>

EMBOX_UNIT_INIT(thread_core_init);


static int id_counter; // TODO make it an indexator


/**
 * Wrapper for thread start routine.
 * Called from sched_switch() function with interrupts off.
 */
static void __attribute__((noreturn)) thread_trampoline(void) {
	struct thread *current = thread_self();
	void *res;

	assert(!critical_allows(CRITICAL_SCHED_LOCK), "0x%x", (uint32_t)__critical_count);

	sched_ack_switched();

	assert(!critical_inside(CRITICAL_SCHED_LOCK));

	/* execute user function handler */
	res = current->run(current->run_arg);
	thread_exit(res);
	/* NOTREACHED */
}

struct thread *thread_create(unsigned int flags, void *(*run)(void *), void *arg) {
	int ret;
	struct thread *t;

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

	/* below we will work with thread's instances and therefore we need to
	 * lock scheduler (disable scheduling) to our structures is not be
	 * corrupted
	 */
	sched_lock();
	{
		/* allocate memory */
		if (!(t = thread_alloc())) {
			t = err_ptr(ENOMEM);
			goto out_unlock;
		}

		/* initialize internal thread structure */
		thread_init(t, flags, run, arg);

		ret = thread_local_alloc(t, MODOPS_THREAD_KEY_QUANTITY);
		if (ret != 0) {
			t = err_ptr(-ret);
			goto out_threadfree;
		}

		/* link with task if needed */
		if (!(flags & THREAD_FLAG_NOTASK)) {
			ret = thread_register(task_self(), t);
			if (ret != 0) {
				t = err_ptr(-ret);
				goto out_localfree;
			}
		}

		thread_cancel_init(t);

		if (!(flags & THREAD_FLAG_SUSPENDED)) {
			thread_launch(t);
		}

		if (flags & THREAD_FLAG_DETACHED) {
			thread_detach(t);
		}

		goto out_unlock;

out_localfree:
		thread_local_free(t);

out_threadfree:
		thread_free(t);

	}
out_unlock:
	sched_unlock();

	return t;
}

void thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg) {
	sched_priority_t priority;

	assert(t);
	assert(run);
	assert(thread_stack_get(t));
	assert(thread_stack_get_size(t));

	t->id = id_counter++; /* setup thread ID */

	dlist_init(&t->thread_link); /* default unlink value */

	t->critical_count = __CRITICAL_COUNT(CRITICAL_SCHED_LOCK);
	t->siglock = 0;
	t->lock = SPIN_UNLOCKED;
	t->ready = false;
	t->active = false;
	t->waiting = true;
	t->state = TS_INIT;

	/* set executive function and arguments pointer */
	t->run = run;
	t->run_arg = arg;

	t->joining = NULL;

	/* calculate current thread priority. It can be change later with
	 * thread_set_priority () function
	 */
	if (flags & THREAD_FLAG_PRIORITY_INHERIT) {
		priority = thread_priority_get(thread_self());
	} else {
		priority = THREAD_PRIORITY_DEFAULT;
	}

	if ((flags & THREAD_FLAG_PRIORITY_LOWER)
			&& (priority > THREAD_PRIORITY_MIN)) {
		priority--;
	} else if ((flags & THREAD_FLAG_PRIORITY_HIGHER)
			&& (priority < THREAD_PRIORITY_HIGH)) {
		priority++;
	}

	/* setup thread priority. Now we have not started thread yet therefore we
	 * just set both base and scheduling priority in default value.
	 */
	thread_priority_init(t, priority);

	/* cpu context init */
	context_init(&t->context, true); /* setup default value of CPU registers */
	context_set_entry(&t->context, thread_trampoline);/*set entry (IP register*/
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
	context_set_stack(&t->context,
			thread_stack_get(t) + thread_stack_get_size(t));

	sigstate_init(&t->sigstate);

	/* Initializes scheduler strategy data of the thread */
	runq_item_init(&t->sched_attr.runq_link);
	sched_affinity_init(t);
	sched_timing_init(t);
}

void thread_delete(struct thread *t) {
	static struct thread *zombie = NULL;

	assert(t);
	assert(t->state & TS_EXITED);

	thread_unregister(t->task, t);
	thread_local_free(t);

	if (zombie) {
		thread_free(zombie);
	}

	if (t != thread_self()) {
		assert(!t->active);
		assert(!t->ready);
		thread_free(t);
		zombie = NULL;
	} else {
		zombie = t;
	}
}

void thread_state_exited(struct thread *t) {
	t->waiting = true;
	t->state |= TS_EXITED;
}

void __attribute__((noreturn)) thread_exit(void *ret) {
	struct thread *current = thread_self();
	struct task *task = task_self();
	struct thread *joining;

	/* We can free only not main threads */
	if (task->main_thread == current) {
		/* We are last thread. */
		task_exit(ret);
		/* NOTREACHED */
	}

	sched_lock();

	// sched_finish(current);
	thread_state_exited(current);

	/* Wake up a joining thread (if any).
	 * Note that joining and run_ret are both in a union. */
	joining = current->joining;
	if (joining) {
		current->run_ret = ret;
		sched_wakeup(joining);
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
		assert(!t->joining);
		assert(!(t->state & TS_DETACHED));

		t->joining = current;
		ret = SCHED_WAIT(t->state & TS_EXITED);
		if (ret) {
			goto out;
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
		assert(!t->joining);
		assert(!(t->state & TS_DETACHED));

		if (!(t->state & TS_EXITED))
			/* The target will free itself upon finishing. */
			t->state |= TS_DETACHED;
		else
			/* The target thread has finished, free it here. */
			thread_delete(t);
	}
	sched_unlock();

	return 0;
}

int thread_launch(struct thread *t) {
	return sched_wakeup(t) ? 0 : -EINVAL;
}

int thread_terminate(struct thread *t) {
	assert(t);

	sched_lock();
	{
		// sched_finish(t);
		// assert(0, "NIY");
		// thread_delete(t);
		sched_freeze(t);
	}
	sched_unlock();

	return 0;
}

void thread_yield(void) {
	sched_post_switch();
}

int thread_set_priority(struct thread *t, sched_priority_t new_priority) {
	// sched_priority_t prior;

	assert(t);
	assert(t->task);

	if ((new_priority < THREAD_PRIORITY_MIN)
			|| (new_priority > THREAD_PRIORITY_MAX)) {
		return -EINVAL;
	}

	// prior = sched_priority_thread(t->task->priority, thread_priority_get(t));
	// if(new_priority != prior) {
	// 	prior = sched_priority_full(t->task->priority, new_priority);
	// 	sched_change_priority(t, prior);
	// }

 	sched_change_priority(t, new_priority);


	return 0;
}

sched_priority_t thread_get_priority(struct thread *t) {
	assert(t);

	return sched_priority_thread(task_get_priority(t->task),
			thread_priority_get(t));
}

clock_t thread_get_running_time(struct thread *t) {
	clock_t running;

	sched_lock();
	{
		running = sched_timing_get(t);

	}
	sched_unlock();

	return running;
}

extern struct thread *idle_thread_create(void);
extern struct thread *boot_thread_create(void);

static int thread_core_init(void) {
	struct thread *idle;
	struct thread *current;

	id_counter = 0; /* start enumeration */

	idle = idle_thread_create(); /* idle thread always has ID=0 */
	current = boot_thread_create(); /* 'init' thread ID=1 */

	return sched_init(idle, current);
}

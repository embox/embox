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
#include <kernel/thread/thread_alloc.h>
#include <kernel/thread/wait_data.h>

#include <kernel/panic.h>

#include <hal/context.h>

EMBOX_UNIT_INIT(thread_core_init);


static int id_counter; // TODO make it an indexator


static void thread_delete(struct thread *t) {
	static struct thread *zombie = NULL;
	struct thread *current = thread_self();

	assert(t);
	assert(thread_state_dead(t->state));
	assert(t != zombie);
	assert(zombie != current);

	if (zombie != NULL) {
		thread_free(zombie);
		zombie = NULL;
	}

	task_remove_thread(t->task, t);

	if (t == current) {
		zombie = t;
	} else {
		thread_free(t);
	}
}


/**
 * Wrapper for thread start routine.
 * Called from sched_switch() function with interrupts off.
 */
static void __attribute__((noreturn)) thread_trampoline(void) {
	struct thread *current = thread_self();
	void *res;

	assert(!critical_allows(CRITICAL_SCHED_LOCK));

	sched_unlock_noswitch();
	ipl_enable();

	assert(!critical_inside(CRITICAL_SCHED_LOCK));

	/* execute user function handler */
	res = current->run(current->run_arg);
	thread_exit(res);
	/* NOTREACHED */
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
	sched_lock();
	{
		/* allocate memory */
		if (!(t = thread_alloc())) {
			res = -ENOMEM;
			goto out;
		}

		/* initialize internal thread structure */
		thread_init(t, flags, run, arg);

		/* link with task if needed */
		if (!(flags & THREAD_FLAG_NOTASK)) {
			task_add_thread(task_self(), t);
		}

		if (!(flags & THREAD_FLAG_SUSPENDED)) {
			thread_launch(t);
		}

		if (flags & THREAD_FLAG_DETACHED) {
			thread_detach(t);
		}
	}
out:
	sched_unlock();

	if (save_ptr) {
		*p_thread = t;
	}

	return res;
}

void thread_init(struct thread *t, unsigned int flags,
		void *(*run)(void *), void *arg) {
	__thread_priority_t priority;

	assert(t);
	assert(run);
	assert(t->stack);
	assert(t->stack_sz);

	t->id = id_counter++; /* setup thread ID */

	dlist_init(&t->thread_link); /* default unlink value */

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
			&& (priority > THREAD_PRIORITY_MIN)) {
		priority--;
	} else if ((flags & THREAD_FLAG_PRIORITY_HIGHER)
			&& (priority < THREAD_PRIORITY_HIGH)) {
		priority++;
	}

	/* setup thread priority. Now we have not started thread yet therefore we
	 * just set both base and scheduling priority in default value.
	 */
	thread_priority_set(t, priority);

	t->joined = NULL; /* there are not any joined thread */

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
	context_set_stack(&t->context, (char *) t->stack + t->stack_sz);

	sched_strategy_init(&t->sched_priv);

	t->affinity = THREAD_AFFINITY_NONE; /* TODO for smp */

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
	assert(!t->joined);

	if (t == current) {
		return -EDEADLK;
	}

	sched_lock();
	{
		if (!thread_state_exited(t->state)) {
			/* Target thread is not exited. Waiting for his exiting. */
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
	assert(!t->joined);

	sched_lock();
	{
		t->state = thread_state_do_detach(t->state);

		if (thread_state_dead(t->state)) {
			/* The target thread has finished, free it here. */
			thread_delete(t);
		}
	}
	sched_unlock();

	return ENOERR;
}

int thread_launch(struct thread *t) {
	int res = ENOERR;

	assert(t);

	sched_lock();
	{
		if (thread_state_started(t->state)) {
			res = -EINVAL;
			goto out;
		}

		if (thread_state_exited(t->state)) {
			res = -ESRCH;
			goto out;
		}

		sched_start(t);
	}
out:
	sched_unlock();

	return res;
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

	return ENOERR;
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

	return ENOERR;
}

thread_priority_t thread_get_priority(struct thread *t) {
	assert(t);

	return thread_priority_get(t);
}

clock_t thread_get_running_time(struct thread *t) {
	clock_t running;

	sched_lock();
	{
		/* if thread is executing now we have to add recent CPU time slice. */
		if (thread_state_oncpu(t->state)) {
			running = clock() - t->last_sync;
			running += t->running_time;
		} else {
			running = t->running_time;
		}
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

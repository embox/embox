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

void main_thread_delete(struct thread *t) {
	static struct thread *zombie = NULL;

	assert(t);
	assert(t->state & TS_EXITED);

	task_thread_unregister(t->task, t);
	thread_local_free(t);
	thread_wait_deinit(&t->thread_wait_list);

	if (zombie) {
		main_thread_free(zombie);
	}

	if (t != thread_self()) {
		assert(!t->schedee.active);
		assert(!t->schedee.ready);
		main_thread_free(t);
		zombie = NULL;
	} else {
		zombie = t;
	}
}

#endif /* OPTION_GET(NUMBER, task_quantity) */


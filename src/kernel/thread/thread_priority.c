/**
 * @file
 *
 * @date Jul 15, 2013
 * @author: Anton Bondarev
 */

#include <kernel/thread.h>
#include <kernel/thread/state.h>
#include <kernel/thread/thread_priority.h>
#include <kernel/task.h>
#include <kernel/sched.h>

int thread_priority_set(struct thread *t, __thread_priority_t new_priority) {
	assert(t);

	sched_lock();
	{
		if (t->state == __THREAD_STATE_INIT) {
			/* if we just create thread (it's not started
			 * we setup default value of thread priorities and return
			 */
			t->thread_priority.initial_priority = new_priority;
			t->thread_priority.sched_priority = t->thread_priority.initial_priority;

			goto out;
		}
		if (t->thread_priority.initial_priority == new_priority) {
			goto out;
		}
		/* if we work with scheduling thread we must take into consideration that priority
		 * can be changed due to inheritance
		 */

		if (t->thread_priority.initial_priority
				== t->thread_priority.sched_priority) {
			t->thread_priority.initial_priority = new_priority;
			t->thread_priority.sched_priority = new_priority;
		} else {
			t->thread_priority.initial_priority = new_priority;
			if (t->thread_priority.sched_priority <= new_priority) {
				t->thread_priority.sched_priority = new_priority;
			}
		}
	}

out:
	sched_unlock();

	return 0;
}

__thread_priority_t thread_priority_get(struct thread *t) {
	assert(t);

	return t->thread_priority.sched_priority;
}

__thread_priority_t thread_priority_inherit(struct thread *t, __thread_priority_t priority) {
	assert(t);

	if(priority > t->thread_priority.sched_priority) {
		t->thread_priority.sched_priority = priority;
	}

	return t->thread_priority.sched_priority;
}

__thread_priority_t thread_priority_reverse(struct thread *t) {
	assert(t);

	t->thread_priority.sched_priority = t->thread_priority.initial_priority;

	return t->thread_priority.sched_priority;
}

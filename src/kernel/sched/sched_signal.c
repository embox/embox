/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#include <kernel/sched.h>
#include <kernel/thread.h>


void sched_signal(struct thread *t) {
	sched_lock();
	{
		if (thread_state_sleeping(t->state)) {
			sched_thread_notify(t, -EINTR);
		} else {
			sched_post_switch();
		}
	}
	sched_unlock();
}

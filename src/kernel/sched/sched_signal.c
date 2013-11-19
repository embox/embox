/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sched/waitq.h>


void sched_signal(struct thread *t) {
	sched_lock();
	{
		if (t->state & __THREAD_STATE_WAITING) {
			waitq_thread_notify(t, -EINTR);
		} else {
			sched_post_switch();
		}
	}
	sched_unlock();
}

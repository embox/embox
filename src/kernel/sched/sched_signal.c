/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sched_wait.h>


int sched_signal(struct thread *t, int type) {
	int res = 0;

	sched_lock();
	{
		if (thread_state_sleeping(t->state)) {
			sched_thread_notify(t, -EINTR);
		} else {
			res = -1;
		}
	}
	sched_unlock();

	return res;
}

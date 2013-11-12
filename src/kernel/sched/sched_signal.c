/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sched/wait_queue.h>


int sched_signal(struct thread *t, int type) {
	int res = 0;

	sched_lock();
	{
		if (t->state & __THREAD_STATE_WAITING) {
			wait_queue_thread_notify(t, -EINTR);
		} else {
			res = -1;
		}
	}
	sched_unlock();

	return res;
}

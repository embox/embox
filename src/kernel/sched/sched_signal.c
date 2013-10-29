/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/sched/wait_queue.h>


void sched_signal(struct thread *t) {
	sched_lock();
	{
		if (thread_state_sleeping(t->state)) {
			wait_queue_thread_notify(t, -EINTR);
		} else {
			sched_post_switch();
		}
	}
	sched_unlock();
}

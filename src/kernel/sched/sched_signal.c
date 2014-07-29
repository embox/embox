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
		if (t->runnable.ready) {
			sched_post_switch();
		} else {
			sched_wakeup(&t->runnable); // XXX -EINTR
		}
	}
	sched_unlock();
}

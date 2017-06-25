/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#include <kernel/sched.h>

void sched_signal(struct schedee *s) {
	sched_lock();
	{
		if (s->ready) {
			sched_post_switch();
		} else {
			sched_wakeup(s); // XXX -EINTR
		}
	}
	sched_unlock();
}

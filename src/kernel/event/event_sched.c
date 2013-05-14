/**
 * @file
 * @brief TODO
 *
 * @date 15.05.2013
 * @author Anton Bulychev
 */

#include <kernel/event.h>
#include <kernel/thread/sched.h>
#include <kernel/thread/wait_queue.h>

void event_init(struct event *event, const char *name) {
	wait_queue_init(&event->wait_queue);
}

void event_notify(struct event *event) {
	wait_queue_notify_all(&event->wait_queue);
}

int __event_wait(unsigned long timeout) {
	if (critical_allows(CRITICAL_SCHED_LOCK)) {
		return sched_wait(timeout);
	} else {
		return sched_wait_locked(timeout);
	}
}

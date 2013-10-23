/**
 * @file
 * @brief TODO
 *
 * @date 15.05.2013
 * @author Anton Bulychev
 */

#include <kernel/event.h>
#include <kernel/sched.h>
#include <kernel/sched/wait_queue.h>

void event_init(struct event *event, const char *name) {
	wait_queue_init(&event->wait_queue);
}

void event_notify(struct event *event) {
	wait_queue_notify_all(&event->wait_queue);
}

int __event_wait(struct event *event, unsigned long timeout) {
	if (critical_allows(CRITICAL_SCHED_LOCK)) {
		return wait_queue_wait(&event->wait_queue, timeout);
	} else {
		return wait_queue_wait_locked(&event->wait_queue, timeout);
	}
}

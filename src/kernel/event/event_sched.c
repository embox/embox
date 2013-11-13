/**
 * @file
 * @brief TODO
 *
 * @date 15.05.2013
 * @author Anton Bulychev
 */

#include <kernel/event.h>
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>

void event_init(struct event *event, const char *name) {
	waitq_init(&event->waitq);
}

void event_notify(struct event *event) {
	waitq_notify_all(&event->waitq);
}

int __event_wait(struct event *event, unsigned long timeout) {
	if (critical_allows(CRITICAL_SCHED_LOCK)) {
		return waitq_wait(&event->waitq, timeout);
	} else {
		return waitq_wait_locked(&event->waitq, timeout);
	}
}

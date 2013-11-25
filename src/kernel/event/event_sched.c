/**
 * @file
 * @brief TODO
 *
 * @date 15.05.2013
 * @author Anton Bulychev
 */

#include <errno.h>

#include <kernel/event.h>
#include <kernel/sched.h>
#include <kernel/sched/waitq.h>

void event_init(struct event *event, const char *name) {
	waitq_init(&event->waitq);
}

void event_notify(struct event *event) {
	sched_wakeup_waitq_all(&event->waitq, ENOERR);
}

/**
 * @file
 * @brief TODO
 *
 * @date 07.09.2012
 * @author Anton Bulychev
 */

#include <kernel/thread/event.h>

void event_init(struct event *e, const char *name) {
	sleepq_init(&e->sleepq);
	slist_link_init(&e->startq_link);
	e->name = name;
}

#if 0
static inline const char *event_name(struct event *e) {
	return e->name;
}
#endif

void event_fire(struct event *e) {
	extern void sched_wake(struct event *event);
	sched_wake(e);
}


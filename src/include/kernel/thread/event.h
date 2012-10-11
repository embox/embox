/**
 * @file
 * @brief This API describes function for work with asynchronous event in
 *      threads
 *
 * @date 08.04.11
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_EVENT_H_
#define KERNEL_THREAD_EVENT_H_

#include <kernel/thread/sched_strategy.h>
#include <util/dlist.h>

#define EVENT_TIMEOUT_INFINITE ((unsigned int)(-1))

struct event_set {
	struct sleepq sleepq;
	struct dlist_head link;
};

struct event {
	const char *name;
	struct dlist_head link;
	struct event_set *set;
};

/* Use it to allocate event_set in global pool and initialize. */
extern struct event_set *event_set_create(void);
void event_set_free(struct event_set *e_set);

/* Use it to initialize event_set if it was already preallocated. */
extern void event_set_init(struct event_set *e_set);
extern void event_set_clear(struct event_set *e_set);

extern int event_wait(struct event_set *e_set, unsigned long timeout);
extern void event_notify(struct event *e);

/* Utils to operate with sets of events */

static inline void event_set_add(struct event_set *e_set, struct event *event) {
	dlist_head_init(&event->link);
	dlist_add_prev(&event->link, &e_set->link);
	event->set = e_set;
}

static inline void event_set_del(struct event *event) {
	dlist_del(&event->link);
	event->set = NULL;
}

#if 0
extern const char *event_name(struct event *event);
extern void event_notify_one(struct event *event);
#endif

#endif /* KERNEL_THREAD_EVENT_H_ */


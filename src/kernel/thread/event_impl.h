/**
 * @file
 * @brief TODO documentation for event_impl.h -- Alina Kramar
 *
 * @date 08.04.11
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_EVENT_IMPL_H_
#define KERNEL_THREAD_EVENT_IMPL_H_


#include <kernel/thread/sched_strategy.h>
#include <util/slist.h>


struct event {
	struct sleepq sleepq;
	struct {
		struct slist_link startq_link;
		int               startq_wake_all;
	} /* unnamed */;   /**< For wakes called inside critical. */
	const char *name;
};

#include <kernel/thread/sched.h>

static inline void event_init(struct event *e, const char *name) {
	sleepq_init(&e->sleepq);
	slist_link_init(&e->startq_link);
	e->name = name;
}

static inline const char *event_name(struct event *e) {
	return e->name;
}

static inline void event_fire(struct event *e) {
	sched_wake(e);
}
#endif /* KERNEL_THREAD_EVENT_IMPL_H_ */

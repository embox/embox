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
	__extension__ struct {
		struct slist_link startq_link;
		int               startq_wake_all;
	} /* unnamed */;   /**< For wakes called inside critical. */
	const char *name;
};

static inline void event_init(struct event *e, const char *name) {
	sleepq_init(&e->sleepq);
	e->name = name;
}

static inline const char *event_name(struct event *e) {
	return e->name;
}

#endif /* KERNEL_THREAD_EVENT_IMPL_H_ */

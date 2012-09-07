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
#include <util/slist.h>

struct event {
	struct sleepq sleepq;
	struct {
		struct slist_link startq_link;
		int               startq_wake_all;
	} /* unnamed */;   /**< For wakes called inside critical. */
	const char *name;
};

extern void event_init(struct event *event, const char *name);

//extern const char *event_name(struct event *event);

extern void event_fire(struct event *event);

#endif /* KERNEL_THREAD_EVENT_H_ */

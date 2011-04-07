/**
 * @file
 * @brief TODO documentation for event_impl.h -- Alina Kramar
 *
 * @date 08.04.2011
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_EVENT_IMPL_H_
#define KERNEL_THREAD_EVENT_IMPL_H_

#include <lib/list.h>

struct event {
	struct list_head sleep_queue;
	const char *name;
};

inline static void event_init(struct event *e, const char *name) {
	INIT_LIST_HEAD(&e->sleep_queue);
	e->name = name;
}

inline static const char *event_name(struct event *e) {
	return e->name;
}

#endif /* KERNEL_THREAD_EVENT_IMPL_H_ */

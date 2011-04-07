/**
 * @file
 * @brief TODO documentation for event.h -- Alina Kramar
 *
 * @date 08.04.2011
 * @author Alina Kramar
 */

#ifndef KERNEL_THREAD_EVENT_H_
#define KERNEL_THREAD_EVENT_H_

#include __impl_x(kernel/thread/event_impl.h)

struct event;

extern void event_init(struct event *event, const char *name);

extern const char *event_name(struct event *event);

#endif /* KERNEL_THREAD_EVENT_H_ */

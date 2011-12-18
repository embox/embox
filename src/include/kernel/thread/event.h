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

#include __impl_x(kernel/thread/event_impl.h)

struct event;

extern void event_init(struct event *event, const char *name);

//extern const char *event_name(struct event *event);

extern void event_fire(struct event *event);

#endif /* KERNEL_THREAD_EVENT_H_ */

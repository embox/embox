/**
 * @file
 * @brief
 *
 * @date 13.05.2013
 * @author Anton Bulychev
 */

#ifndef KERNEL_EVENT_H_
#define KERNEL_EVENT_H_

#include <module/embox/kernel/event_api.h>

extern void event_init(struct event *event, const char *name);

extern void event_notify(struct event *event);

#endif /* KERNEL_EVENT_H_ */

/**
 * @file
 * @brief
 *
 * @date 21.06.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_MANUAL_EVENT_H_
#define KERNEL_MANUAL_EVENT_H_

#include <module/embox/kernel/manual_event_api.h>

struct manual_event;

extern void manual_event_init(struct manual_event *m_event,
		int set);
extern void manual_event_reset(struct manual_event *m_event);
extern void manual_event_set(struct manual_event *m_event);
extern void manual_event_notify(struct manual_event *m_event);
extern int manual_event_wait(struct manual_event *m_event,
		unsigned long timeout);

#endif /* KERNEL_MANUAL_EVENT_H_ */

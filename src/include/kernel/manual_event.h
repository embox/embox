/**
 * @file
 * @brief
 *
 * @date 21.06.13
 * @author Ilia Vaprol
 */

#ifndef KERNEL_MANUAL_EVENT_H_
#define KERNEL_MANUAL_EVENT_H_

#include <kernel/event.h>

struct manual_event {
	struct event event;
	int state;
};

static inline void manual_event_init(struct manual_event *m_event,
		int state) {
	event_init(&m_event->event, NULL);
	m_event->state = state;
}

static inline void manual_event_reset(struct manual_event *m_event) {
	m_event->state = 0;
}

static inline void manual_event_set(struct manual_event *m_event) {
	m_event->state = 1;
}

static inline void manual_event_notify(struct manual_event *m_event) {
	if (!m_event->state) {
		manual_event_set(m_event);
		event_notify(&m_event->event);
	}
}

static inline int manual_event_wait(struct manual_event *m_event,
		unsigned long timeout) {
	return EVENT_WAIT_OR_INTR(&m_event->event, m_event->state, timeout);
}

#endif /* KERNEL_MANUAL_EVENT_H_ */

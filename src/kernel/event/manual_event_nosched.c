/**
 * @file
 * @brief
 *
 * @date 22.06.13
 * @author Ilia Vaprol
 */

#include <kernel/manual_event.h>

void manual_event_init(struct manual_event *m_event, int set) {
}

void manual_event_reset(struct manual_event *m_event) {
}

void manual_event_set(struct manual_event *m_event) {
}

int manual_event_is_set(struct manual_event *m_event) {
	return 0;
}

void manual_event_notify(struct manual_event *m_event) {
}

int manual_event_wait(struct manual_event *m_event,
		unsigned long timeout) {
	return 0;
}

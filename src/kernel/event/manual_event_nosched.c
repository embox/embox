/**
 * @file
 * @brief
 *
 * @date 22.06.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <kernel/manual_event.h>
#include <kernel/softirq_lock.h>
#include <stddef.h>

void manual_event_init(struct manual_event *m_event, int set) {
	assert(m_event != NULL);
	m_event->wait = 1;
	m_event->set = set;
}

void manual_event_reset(struct manual_event *m_event) {
	assert(m_event != NULL);
	m_event->set = 0;
}

void manual_event_set(struct manual_event *m_event) {
	assert(m_event != NULL);
	m_event->set = 1;
}

int manual_event_is_set(struct manual_event *m_event) {
	assert(m_event != NULL);
	return m_event->set;
}

void manual_event_notify(struct manual_event *m_event) {
	assert(m_event != NULL);
	if (!m_event->set) {
		m_event->wait = 0;
	}
}

void manual_event_set_and_notify(struct manual_event *m_event) {
	assert(m_event != NULL);
	if (!m_event->set) {
		m_event->set = 1;
		m_event->wait = 0;
	}
}

int manual_event_wait(struct manual_event *m_event,
		unsigned long timeout) {
	assert(m_event != NULL);

	softirq_lock();
	{
		if (!m_event->set) {
			while (m_event->wait); /* TODO timeout */
			m_event->wait = 1;
		}
		else {
		}
	}
	softirq_unlock();

	return 0;
}

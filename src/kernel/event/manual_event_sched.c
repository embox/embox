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
#include <kernel/sched.h>
#include <kernel/sched/wait_queue.h>

void manual_event_init(struct manual_event *m_event, int set) {
	assert(m_event != NULL);
	wait_queue_init(&m_event->wait_queue);
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
		wait_queue_notify_all(&m_event->wait_queue);
	}
}

void manual_event_set_and_notify(struct manual_event *m_event) {
	assert(m_event != NULL);
	if (!m_event->set) {
		m_event->set = 1;
		wait_queue_notify_all(&m_event->wait_queue);
	}
}

static int __manual_event_wait(struct manual_event *m_event,
		unsigned long timeout) {
	int ret;

	softirq_unlock();
	{
		if (critical_allows(CRITICAL_SCHED_LOCK)) {
			ret = wait_queue_wait(&m_event->wait_queue, timeout);
		}
		else {
			ret = wait_queue_wait_locked(&m_event->wait_queue, timeout);
		}
	}
	softirq_lock();

	return ret;
}

int manual_event_wait(struct manual_event *m_event,
		unsigned long timeout) {
	int ret;

	assert(m_event != NULL);
	assert(critical_allows(CRITICAL_SOFTIRQ_LOCK));

	softirq_lock();
	{
		if (!m_event->set) {
			ret = __manual_event_wait(m_event, timeout);
		}
		else {
			ret = 0;
		}
	}
	softirq_unlock();

	return ret;
}

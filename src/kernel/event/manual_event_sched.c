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
#include <kernel/sched_wait.h>

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

static int __manual_event_wait(unsigned long timeout) {
	int ret;

	softirq_unlock();
	{
		if (critical_allows(CRITICAL_SCHED_LOCK)) {
			ret = sched_wait(timeout);
		}
		else {
			ret = sched_wait_locked(timeout);
		}
	}
	softirq_lock();

	return ret;
}

int manual_event_wait(struct manual_event *m_event,
		unsigned long timeout) {
	int ret;
	struct wait_link wait_lnk;

	assert(m_event != NULL);
	assert(critical_allows(CRITICAL_SOFTIRQ_LOCK));

	softirq_lock();
	{
		if (!m_event->set) {
			wait_queue_prepare(&wait_lnk);
			wait_queue_insert(&m_event->wait_queue, &wait_lnk);

			ret = __manual_event_wait(timeout);

			wait_queue_cleanup(&wait_lnk);
		}
		else {
			ret = 0;
		}
	}
	softirq_unlock();

	return ret;
}

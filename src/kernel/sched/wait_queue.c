/**
 * @file
 * @brief
 *
 * @date 06.05.2013
 * @author Anton Bulychev
 */

#include <errno.h>

#include <hal/ipl.h>

#include <kernel/irq.h>
#include <kernel/thread.h>
#include <kernel/sched.h>
#include <kernel/sched/wait_queue.h>

#include <util/member.h>
#include <util/dlist.h>

#include <kernel/thread/state.h>
#include <kernel/critical.h>
#include <kernel/time/timer.h>
#include <assert.h>

static inline int in_harder_critical(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

static inline int in_sched_locked(void) {
	return !critical_allows(CRITICAL_SCHED_LOCK);
}

static void timeout_handler(struct sys_timer *timer, void *sleep_data) {
	struct thread *thread = (struct thread *) sleep_data;
	wait_queue_thread_notify(thread, -ETIMEDOUT);
}

static int sched_wait_locked(unsigned long timeout) {
	int ret;
	struct sys_timer tmr;
	struct thread *current = thread_get_current();

	assert(in_sched_locked() && !in_harder_critical());
	assert(thread_state_running(current->state));
	//there was an assert /* Should be prepared */

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		ret = timer_init(&tmr, TIMER_ONESHOT, (uint32_t)timeout, timeout_handler, current);
		if (ret != ENOERR) {
			return ret;
		}
	}

	sched_sleep(current);

	sched_unlock();

	/* At this point we have been awakened and are ready to go. */
	assert(!in_sched_locked());
	assert(thread_state_running(current->state));

	sched_lock();

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		timer_close(&tmr);
	}

	return current->wait_link->result;
}

void wait_queue_remove(struct wait_link *wait_link) {
	ipl_t ipl = ipl_save();
	{
		dlist_del(&wait_link->link);
	}
	ipl_restore(ipl);
}

void wait_queue_insert(struct wait_queue *wait_queue,
		struct wait_link *wait_link) {
	ipl_t ipl = ipl_save();
	{
		dlist_head_init(&wait_link->link);
		wait_link->thread = thread_self();

		dlist_add_prev(&wait_link->link, &wait_queue->list);
	}
	ipl_restore(ipl);
}

void wait_queue_prepare(struct wait_link *wait_link) {
	struct thread *current = thread_get_current();

	wait_link->thread = current;
	wait_link->result = 0;
	current->wait_link = wait_link;
}

void wait_queue_cleanup(struct wait_link *wait_link) {
	//TODO: delete this
}

int wait_queue_wait(struct wait_queue *wait_queue, int timeout) {
	int result;

	sched_lock();
	{
		result = wait_queue_wait_locked(wait_queue, timeout);
	}
	sched_unlock();

	return result;
}

int wait_queue_wait_locked(struct wait_queue *wait_queue, int timeout) {
	struct wait_link wait_link;
	int result;

	wait_queue_insert(wait_queue, &wait_link);

	wait_queue_prepare(&wait_link);

	result = sched_wait_locked(timeout);

	wait_queue_cleanup(&wait_link);

	return result;
}

void wait_queue_thread_notify(struct thread *thread, int result) {
	assert(thread);

	irq_lock();
	{
		if (thread_state_sleeping(thread->state)) {
			thread->wait_link->result = result;

			sched_wake(thread);

			wait_queue_remove(thread->wait_link);
		}
	}
	irq_unlock();
}

void wait_queue_notify(struct wait_queue *wait_queue) {
	struct wait_link *link, *next;
	struct thread *t;

	ipl_t ipl = ipl_save();
	{
		if (dlist_empty(&wait_queue->list)) {
			goto out;
		}
		t = dlist_entry(wait_queue->list.next, struct wait_link, link)->thread;

		dlist_foreach_entry(link, next, &wait_queue->list, link) {
			if (thread_priority_get(link->thread) > thread_priority_get(t)) {
				t = link->thread;
			}
		}

		wait_queue_thread_notify(t, ENOERR);
	}
out:
	ipl_restore(ipl);
}

void wait_queue_notify_all_err(struct wait_queue *wait_queue, int error) {
	struct wait_link *link, *next;

	ipl_t ipl = ipl_save();
	{
		dlist_foreach_entry(link, next, &wait_queue->list, link) {
			wait_queue_thread_notify(link->thread, error);
		}
	}
	ipl_restore(ipl);
}

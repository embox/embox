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
#include <kernel/sched/waitq.h>

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
	waitq_thread_notify(thread, -ETIMEDOUT);
}

int __waitq_wait_locked(int timeout) {
	int ret;
	struct sys_timer tmr;
	struct thread *current = thread_get_current();

	assert(in_sched_locked() && !in_harder_critical());
	assert(current->wait_link); /* Should be prepared */

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		ret = timer_init(&tmr, TIMER_ONESHOT, (uint32_t)timeout, timeout_handler, current);
		if (ret != ENOERR) {
			return ret;
		}
	}

	/* __THREAD_STATE_WAITING flag sets in prepare stage */
	sched_post_switch();
	sched_unlock();

	/* At this point we have been awakened and are ready to go. */
	assert(!in_sched_locked());
	assert(current->state & __THREAD_STATE_RUNNING);

	sched_lock();

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		timer_close(&tmr);
	}

	return current->wait_link->result;
}

void waitq_remove(struct wait_link *wait_link) {
	ipl_t ipl = ipl_save();
	{
		dlist_del(&wait_link->link);
	}
	ipl_restore(ipl);
}

static void waitq_insert(struct waitq *waitq,
		struct wait_link *wait_link) {
	ipl_t ipl = ipl_save();
	{
		dlist_head_init(&wait_link->link);
		wait_link->thread = thread_self();
		dlist_add_prev(&wait_link->link, &waitq->list);
	}
	ipl_restore(ipl);
}

void __waitq_prepare(struct waitq *waitq, struct wait_link *wait_link) {
	struct thread *current = thread_get_current();

	assert(!current->wait_link);
	assert(current->state & (__THREAD_STATE_RUNNING));

	/* TODO check it but should work */
	current->state |= __THREAD_STATE_WAITING;
	current->state &= ~(__THREAD_STATE_READY | __THREAD_STATE_RUNNING);

	wait_link->thread = current;
	wait_link->result = 0;
	current->wait_link = wait_link;

	waitq_insert(waitq, wait_link);
}

void __waitq_cleanup(void) {
	thread_get_current()->wait_link = 0;
}

int waitq_wait(struct waitq *waitq, int timeout) {
	int result;

	sched_lock();
	{
		result = waitq_wait_locked(waitq, timeout);
	}
	sched_unlock();

	return result;
}

int waitq_wait_locked(struct waitq *waitq, int timeout) {
	struct wait_link wait_link;
	int result;

	__waitq_prepare(waitq, &wait_link);

	result = __waitq_wait_locked(timeout);

	__waitq_cleanup();

	return result;
}

int __waitq_wait(int timeout) {
	int result;

	sched_lock();
	{
		result = __waitq_wait_locked(timeout);
	}
	sched_unlock();

	return result;
}

void waitq_thread_notify(struct thread *thread, int result) {
	assert(thread);
	assert(__THREAD_STATE_WAITING & thread->state);

	irq_lock();
	{
		thread->wait_link->result = result;

		sched_wake(thread);

		waitq_remove(thread->wait_link);
	}
	irq_unlock();
}

void waitq_notify(struct waitq *waitq) {
	struct wait_link *link, *next;
	struct thread *t;

	ipl_t ipl = ipl_save();
	{
		if (dlist_empty(&waitq->list)) {
			goto out;
		}
		t = dlist_entry(waitq->list.next, struct wait_link, link)->thread;

		dlist_foreach_entry(link, next, &waitq->list, link) {
			if (thread_priority_get(link->thread) > thread_priority_get(t)) {
				t = link->thread;
			}
		}

		waitq_thread_notify(t, ENOERR);
	}
out:
	ipl_restore(ipl);
}

void waitq_notify_all_err(struct waitq *waitq, int error) {
	struct wait_link *link, *next;

	ipl_t ipl = ipl_save();
	{
		dlist_foreach_entry(link, next, &waitq->list, link) {
			waitq_thread_notify(link->thread, error);
		}
	}
	ipl_restore(ipl);
}

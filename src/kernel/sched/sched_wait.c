/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#include <kernel/thread/state.h>
#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/irq.h>
#include <kernel/critical.h>
#include <kernel/time/timer.h>
#include <util/member.h>
#include <hal/ipl.h>
#include <assert.h>
#include <errno.h>



struct work_queue startq;

static inline int in_harder_critical(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

static inline int in_sched_locked(void) {
	return !critical_allows(CRITICAL_SCHED_LOCK);
}

void sched_thread_notify(struct thread *thread, int result) {
	assert(thread);

	irq_lock();
	{
		if (thread_state_sleeping(thread->state)) {
			thread->wait_data.result = result;

			sched_wake(thread);

			if (thread->wait_data.on_notified) {
				thread->wait_data.on_notified(thread, thread->wait_data.data);
			}

			//sched_post_switch();
		}
	}
	irq_unlock();
}

void sched_prepare_wait(void (*on_notified)(struct thread *, void *),
		void *data) {
	struct thread *current = thread_get_current();
	struct wait_data *wait_data = &current->wait_data;
	ipl_t ipl;

	wait_data->data = data;
	wait_data->on_notified = on_notified;

	assert(!thread_state_sleeping(current->state));

	ipl = ipl_save();
	{
		wait_data->result = ENOERR;
	}
	ipl_restore(ipl);
}

static void timeout_handler(struct sys_timer *timer, void *sleep_data) {
	struct thread *thread = (struct thread *) sleep_data;
	sched_thread_notify(thread, -ETIMEDOUT);
}

int sched_wait_locked(unsigned long timeout) {
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

	return current->wait_data.result;
}

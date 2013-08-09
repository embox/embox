/**
 * @file
 *
 * @date Aug 8, 2013
 * @author: Anton Bondarev
 */

#include <kernel/sched.h>
#include <kernel/thread.h>
#include <kernel/irq.h>
#include <kernel/critical.h>
#include <kernel/time/timer.h>
#include <util/member.h>




struct work_queue startq;

static inline int in_harder_critical(void) {
	return critical_inside(__CRITICAL_HARDER(CRITICAL_SCHED_LOCK));
}

static inline int in_sched_locked(void) {
	return !critical_allows(CRITICAL_SCHED_LOCK);
}


int sched_wait_init(void){
	work_queue_init(&startq);

	return 0;
}

void sched_wait_run(void) {
	work_queue_run(&startq);
}

void sched_thread_notify(struct thread *thread, int result) {
	irq_lock();
	{
		if (thread->wait_data.status == WAIT_DATA_STATUS_WAITING) {
			work_post(&thread->wait_data.work, &startq);

			thread->wait_data.status = WAIT_DATA_STATUS_NOTIFIED;
			thread->wait_data.result = result;

			sched_post_switch();
		}
	}
	irq_unlock();
}


static int notify_work(struct work *work) {
	struct wait_data *wait_data = member_cast_out(work, struct wait_data, work);
	struct thread *t = member_cast_out(wait_data, struct thread, wait_data);

	assert(in_sched_locked());

	sched_wake(t);

	if (wait_data->on_notified) {
		wait_data->on_notified(t, wait_data->data);
	}

	return 1;
}

void sched_prepare_wait(notify_handler on_notified, void *data) {
	struct wait_data *wait_data = &thread_get_current()->wait_data;

	wait_data->data = data;
	wait_data->on_notified = on_notified;

	wait_data_prepare(wait_data, &notify_work);
}

void sched_cleanup_wait(void) {
	wait_data_cleanup(&thread_get_current()->wait_data);
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
	assert(current->wait_data.status != WAIT_DATA_STATUS_NONE); /* Should be prepared */

	if (timeout != SCHED_TIMEOUT_INFINITE) {
		ret = timer_init(&tmr, TIMER_ONESHOT, (uint32_t)timeout, timeout_handler, current);
		if (ret != ENOERR) {
			return ret;
		}
	}

	work_enable(&current->wait_data.work);
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

int sched_wait(unsigned long timeout) {
	int sleep_res;
	assert(!in_sched_locked());

	sched_lock();
	{
		sleep_res = sched_wait_locked(timeout);
	}
	sched_unlock();

	return sleep_res;
}

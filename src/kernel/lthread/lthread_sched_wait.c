/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    12.08.2014
 */

#include <assert.h>
#include <stdbool.h>

#include <kernel/spinlock.h>
#include <kernel/sched.h>
#include <kernel/time/timer.h>

#include <kernel/schedee/current.h>
#include <kernel/schedee/schedee.h>
#include <kernel/lthread/lthread.h>

static void sched_wait_timeout_handler(struct sys_timer *timer, void *data) {
	struct schedee *s = data;
	sched_wakeup(s);
}

void sched_wait_info_init(struct sched_wait_info *info) {
	info->prev_time = 0;
	info->cur_time = 0;
	info->remain = 0;
	info->status = 0;
}

void sched_wait_prepare_lthread(void) {
	struct sched_wait_info *info = &lthread_self()->info;

	sched_wait_prepare_schedee();

	info->remain = 0;

	if (info->status == SCHED_WAIT_STARTED) {
		info->prev_time = info->cur_time;
	}

	info->cur_time = clock();
}

void sched_wait_cleanup_lthread(void) {
	struct sched_wait_info *info = &lthread_self()->info;

	info->prev_time = 0;
	info->cur_time = 0;
	info->remain = 0;
	info->status = 0;

	sched_wait_cleanup_schedee();
}

int sched_wait_lthread(void) {
	struct sched_wait_info *info = &lthread_self()->info;

	if (info->status == SCHED_WAIT_STARTED) {
		info->status = SCHED_WAIT_FINISHED;
		return 0;
	}

	info->status = SCHED_WAIT_STARTED;
	return -EAGAIN;
}

int sched_wait_timeout_lthread(clock_t timeout) {
	struct lthread *lt = lthread_self();
	struct sched_wait_info *info = &lt->info;
	int res, diff;

	if (timeout == SCHED_TIMEOUT_INFINITE) {
		return sched_wait_lthread();
	}

	if (info->status == SCHED_WAIT_STARTED) {
		info->status = SCHED_WAIT_FINISHED;

		timer_close(&info->tmr);

		diff = info->cur_time - info->prev_time;

		if (diff < timeout) {
			info->remain = info->remain - diff;
			return 0;

		} else {
			info->remain = 0;
			return -ETIMEDOUT;
		}
	}

	if ((res = timer_init(&info->tmr, TIMER_ONESHOT, jiffies2ms(timeout),
			sched_wait_timeout_handler, &lt->schedee))) {
		return res;
	}

	info->status = SCHED_WAIT_STARTED;
	return -EAGAIN;
}


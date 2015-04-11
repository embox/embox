/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    12.08.2014
 */

#include <errno.h>
#include <util/math.h>
#include <kernel/time/timer.h>

#include <kernel/lthread/lthread.h>

static void sched_wait_timeout_handler(struct sys_timer *timer, void *data) {
	struct schedee *s = data;
	sched_wakeup(s);
}

static void sched_wait_info_clear(struct sched_wait_info *info) {
	info->last_sync = 0;
	info->remain = 0;
	info->status = SCHED_WAIT_FINISHED;
	info->tmr = NULL;
}

void sched_wait_info_init(struct sched_wait_info *info) {
	sched_wait_info_clear(info);
}

void sched_wait_prepare_lthread(struct lthread *self, clock_t timeout) {
	struct sched_wait_info *info = &self->info;
	clock_t cur_time;
	int diff;

	sched_wait_prepare();

	if (info->status != SCHED_WAIT_STARTED) {
		if (!info->remain)
			info->remain = timeout;

		info->last_sync = clock();
	} else if (timeout != SCHED_TIMEOUT_INFINITE) {
		cur_time = clock();
		diff = cur_time - info->last_sync;
		info->last_sync = cur_time;

	 	info->remain = max((int)info->remain - diff, 0);
	}
}

void sched_wait_cleanup_lthread(struct lthread *self) {
	struct sched_wait_info *info = &self->info;

	if (info->status == SCHED_WAIT_STARTED &&
			info->remain != SCHED_TIMEOUT_INFINITE) {
		timer_close(info->tmr);
	}

	sched_wait_info_clear(info);
	sched_wait_cleanup();
}

static int sched_wait_lthread(struct lthread *self) {
	struct sched_wait_info *info = &self->info;

	if (info->status == SCHED_WAIT_STARTED) {
		info->status = SCHED_WAIT_FINISHED;
		return 0;
	}

	info->status = SCHED_WAIT_STARTED;
	return -EAGAIN;
}

int sched_wait_timeout_lthread(struct lthread *self, clock_t *remain) {
	struct sched_wait_info *info = &self->info;
	int res;

	if (info->remain == SCHED_TIMEOUT_INFINITE) {
		return sched_wait_lthread(self);
	}

	if (info->status == SCHED_WAIT_STARTED) {
		timer_close(info->tmr);

		info->status = SCHED_WAIT_FINISHED;

		if (remain) {
			*remain = info->remain;
		}

		return info->remain ? 0 : -ETIMEDOUT;
	}

	if ((res = timer_set(&info->tmr, TIMER_ONESHOT, jiffies2ms(info->remain),
			sched_wait_timeout_handler, &self->schedee))) {
		return res;
	}

	info->status = SCHED_WAIT_STARTED;
	return -EAGAIN;
}

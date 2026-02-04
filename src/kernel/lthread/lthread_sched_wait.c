/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    12.08.2014
 */

#include <errno.h>
#include <util/math.h>
#include <hal/clock.h>
#include <kernel/time/sys_timer.h>

#include <kernel/lthread/lthread.h>

static void sched_wait_timeout_handler(struct sys_timer *timer, void *data) {
	struct schedee *s = data;
	sched_wakeup(s);
}

static void sched_wait_info_clear(struct sched_wait_info *info) {
	info->last_sync = 0;
	info->remain = 0;
	info->status = 0;
	info->tmr = NULL;
}

void sched_wait_info_init(struct sched_wait_info *info) {
	sched_wait_info_clear(info);
}

void sched_wait_prepare_lthread(struct lthread *self, clock_t timeout) {
	struct sched_wait_info *info = &self->info;
	clock_t cur_time = clock_sys_ticks();

	if (!(info->status & SCHED_WAIT_PREPARED)) {
		info->status |= SCHED_WAIT_PREPARED;
		info->remain = timeout;
	}

	if (info->status & SCHED_WAIT_STARTED &&
			info->remain != SCHED_TIMEOUT_INFINITE) {
		int diff = cur_time - info->last_sync;
	 	info->remain = max((int)info->remain - diff, 0);
	}

	info->last_sync = cur_time;
}

void sched_wait_cleanup_lthread(struct lthread *self) {
	struct sched_wait_info *info = &self->info;

	if (info->status & SCHED_WAIT_STARTED &&
			info->remain != SCHED_TIMEOUT_INFINITE) {
		sys_timer_close(info->tmr);
	}

	sched_wait_info_clear(info);
}

static int sched_wait_lthread(struct lthread *self) {
	struct sched_wait_info *info = &self->info;

	if (info->status & SCHED_WAIT_STARTED) {
		info->status &= ~SCHED_WAIT_STARTED;
		return 0;
	}

	info->status |= SCHED_WAIT_STARTED;
	return -EAGAIN;
}

int sched_wait_timeout_lthread(struct lthread *self, clock_t *remain) {
	struct sched_wait_info *info = &self->info;
	int res;

	if (info->remain == SCHED_TIMEOUT_INFINITE) {
		return sched_wait_lthread(self);
	}

	if (info->status & SCHED_WAIT_STARTED) {
		sys_timer_close(info->tmr);

		info->status &= ~SCHED_WAIT_STARTED;

		if (remain) {
			*remain = info->remain;
		}

		return info->remain ? 0 : -ETIMEDOUT;
	}

	if ((res = sys_timer_set(&info->tmr, SYS_TIMER_ONESHOT, jiffies2ms(info->remain),
			sched_wait_timeout_handler, &self->schedee))) {
		return res;
	}

	info->status |= SCHED_WAIT_STARTED;
	return -EAGAIN;
}

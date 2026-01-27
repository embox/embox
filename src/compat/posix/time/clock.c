/**
 * @file
 *
 * @brief ISO C99 Standard: 7.23 Date and time.
 *
 * @date 12.05.12
 * @author Ilia Vaprol
 * @author Anton Bondarev
 *         - clock()
 */

#include <errno.h>
#include <time.h>
#include <hal/clock.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>
#include <kernel/time/time.h>

clock_t clock(void) {
	return clock_sys_ticks();
}

int clock_gettime(clockid_t clk_id, struct timespec *ts) {
	switch (clk_id) {
	case CLOCK_REALTIME:
		getnsofday(ts, NULL);
		break;
	case CLOCK_MONOTONIC:
		ktime_get_timespec(ts);
		break;
	default:
		return SET_ERRNO(EINVAL);
	}

	return 0;
}

int clock_settime(clockid_t clk_id, const struct timespec *ts) {
	switch (clk_id) {
	case CLOCK_REALTIME:
		setnsofday(ts, NULL);
		break;
	default:
		return SET_ERRNO(EINVAL);
	}
	return 0;
}

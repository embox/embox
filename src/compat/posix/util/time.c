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

#include <time.h>
#include <hal/clock.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>


clock_t clock(void) {
	return clock_sys_ticks();
}

static struct timespec *ns2timespec(time64_t ns, struct timespec *ts) {
	ts->tv_sec = ns / NSEC_PER_SEC;
	ts->tv_nsec = ns % NSEC_PER_SEC;
	return ts;
}

int clock_gettime(clockid_t clk_id, struct timespec *ts) {

	ns2timespec(ktime_get_ns(), ts);
	return 0;
}

time_t time(time_t *t) {
	time_t sec;

	sec = ktime_get_ns() / NSEC_PER_SEC;
	if (t != NULL) {
		*t = sec;
	}

	return sec;
}

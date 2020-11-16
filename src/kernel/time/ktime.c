/**
 * @file
 *
 * @brief Kernel time implementation.
 * @details Kernel time base on mostly precise clock source. It
 *
 * @date 18.05.2012
 * @author Anton Bondarev
 */

#include <kernel/time/itimer.h>
#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

static struct itimer sys_timecounter;
struct clock_source *kernel_clock_source;

time64_t ktime_get_ns(void) {
	struct timespec ts;

	itimer_read_timespec(&sys_timecounter, &ts);

	return timespec_to_ns(&ts);
}

struct timeval *ktime_get_timeval(struct timeval *tv) {
	struct timespec ts;

	itimer_read_timespec(&sys_timecounter, &ts);

	TIMESPEC_TO_TIMEVAL(tv, &ts);

	return tv;
}

struct timespec *ktime_get_timespec(struct timespec *ts) {
	itimer_read_timespec(&sys_timecounter, ts);
	return ts;
}

/* TODO Use only in jffs2.h as "#define timestamp ktime_get_timeseconds" */
time_t ktime_get_timeseconds(void) {
	struct timespec ts;

	ktime_get_timespec (&ts);
	return ts.tv_sec;
}

int monotonic_clock_select(void) {
	/* find clock_event_device with maximal frequency  */
	kernel_clock_source = clock_source_get_best(CS_ANY);
	assert(kernel_clock_source);

	itimer_init(&sys_timecounter, kernel_clock_source, 0);

	return 0;
}

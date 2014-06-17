/**
 * @file
 *
 * @brief Kernel time implementation.
 * @details Kernel time base on mostly precise clock source. It
 *
 * @date 18.05.2012
 * @author Anton Bondarev
 */
#include <embox/unit.h>

#include <kernel/time/itimer.h>
#include <kernel/time/ktime.h>
#include <kernel/time/clock_source.h>
#include <kernel/time/time.h>

EMBOX_UNIT_INIT(module_init);

static struct itimer sys_timecounter;
struct clock_source *kernel_clock_source;

time64_t ktime_get_ns(void) {
	return itimer_read(&sys_timecounter);
}

struct timeval *ktime_get_timeval(struct timeval *tv) {
	time64_t ns = ktime_get_ns();
	*tv = ns_to_timeval(ns);
	return tv;
}

struct timespec *ktime_get_timespec(struct timespec *ts) {
	time64_t ns = ktime_get_ns();

	*ts = ns_to_timespec(ns);
	return ts;
}

time_t ktime_get_timeseconds(void) {
	struct timespec ts;

	ktime_get_timespec (&ts);
	return ts.tv_sec;
}

static int module_init(void) {
	/* find clock_event_device with maximal frequency  */
	kernel_clock_source = clock_source_get_best(CS_ANY);
	assert(kernel_clock_source);

	itimer_init(&sys_timecounter, kernel_clock_source, 0);

	return 0;
}

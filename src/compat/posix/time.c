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
#if 0
int clock_getres(clockid_t clk_id, struct timespec *res) {
	struct clock_source *cs;
	cs =  clock_source_get_default();
	if(NULL != res) {
		//TODO fill res structure
	}

	return cs->resolution;
}
#endif

static struct timespec *ns2timespec(ns_t ns, struct timespec *ts) {
	ts->tv_sec = ns / NSEC_PER_SEC;
	ts->tv_nsec = ns % NSEC_PER_SEC;
	return ts;
}

int clock_gettime(clockid_t clk_id, struct timespec *ts) {
//	struct clock_source *cs;

//	cs =  clock_source_get_default();

	ns2timespec(ktime_get_ns(), ts);
	return 0;
}

#if 0
int clock_settime(clockid_t clk_id, const struct timespec *tp) {
	struct clock_source *cs;
	cs =  clock_source_get_default();
	return 0;
}
#endif

time_t time(time_t *t) {
	time_t sec;

	sec = ktime_get_ns() / NSEC_PER_SEC;
	if (t != NULL) {
		*t = sec;
	}

	return sec;
}


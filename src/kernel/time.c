/**
 * @file
 * @brief ISO C99 Standard: 7.23 Date and time.
 *
 * @date 06.07.11
 * @author Ilia Vaprol
 */

#include <time.h>
#include <kernel/timer.h>
#include <kernel/clock_source.h>

static
useconds_t time_usec(void) {
	return clock_source_clock_to_usec(cnt_system_time());
}

clock_t clock(void) {
	return cnt_system_time();
}

struct timeval * get_timeval(struct timeval *tv) {
	useconds_t usec;

	usec = time_usec();
	tv->tv_sec = usec / MICROSEC_PER_SEC;
	tv->tv_usec = usec % MICROSEC_PER_SEC;
	return tv;
}

extern time_t time(time_t *t) {
	time_t sec;

	sec = time_usec() / MICROSEC_PER_SEC;
	if (t) {
		*t = sec;
	}
	return sec;
}

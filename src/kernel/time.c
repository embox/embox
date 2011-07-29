/**
 * @file
 * @brief ISO C99 Standard: 7.23 Date and time.
 *
 * @date 06.07.11
 * @author Ilia Vaprol
 */

#include <time.h>
#include <kernel/clock_source.h>

extern clock_t sys_ticks; /* Clocks after start system. Increments in kernel/timer.c */

static
useconds_t time_usec(void) {
	return clock_source_clock_to_usec(sys_ticks);
}

clock_t clock(void) {
	return sys_ticks;
}

struct timeval * get_timeval(struct timeval *tv) {
	useconds_t usec;

	usec = time_usec();
	tv->tv_sec = usec / MICROSEC_PER_SEC;
	tv->tv_usec = usec % MICROSEC_PER_SEC;
	return tv;
}

time_t time(time_t *t) {
	time_t sec;

	sec = time_usec() / MICROSEC_PER_SEC;
	if (t) {
		*t = sec;
	}
	return sec;
}

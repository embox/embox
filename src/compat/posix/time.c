/**
 * @file
 *
 * @brief ISO C99 Standard: 7.23 Date and time.
 *
 * @date 12.05.2012
 * @author Ilia Vaprol
 * @author Anton Bondarev
 *         - clock()
 */

#include <time.h>
#include <hal/clock.h>
#include <kernel/clock_source.h>

clock_t clock(void) {
	return clock_sys_ticks();
}

#if 0
time_t time(time_t *t) {
	time_t sec;

	sec = time_usec() / MICROSEC_PER_SEC;
	if (t) {
		*t = sec;
	}
	return sec;
}
#endif

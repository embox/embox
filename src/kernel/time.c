/**
 * @file
 * @brief ISO C99 Standard: 7.23 Date and time.
 *
 * @date 06.07.11
 * @author Ilia Vaprol
 */

#include <time.h>
#include <kernel/timer.h>

clock_t clock(void) {
	return (clock_t) ((cnt_system_time() * CLOCKS_PER_SEC) / TIMER_FREQUENCY);
}

struct timeval * get_timeval(struct timeval *now) {
	uint32_t n = cnt_system_time();
	now->tv_sec = n / TIMER_FREQUENCY;
	now->tv_usec = ((n % TIMER_FREQUENCY) * CLOCKS_PER_SEC) / TIMER_FREQUENCY;
	return now;
}

time_t time(time_t *now) {
	if (now) {
		*now = cnt_system_time() / TIMER_FREQUENCY;
		return *now;
	}
	return (time_t) (cnt_system_time() / TIMER_FREQUENCY);
}

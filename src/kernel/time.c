/**
 * @file
 * @brief ISO C99 Standard: 7.23 Date and time.
 *
 * @date 06.07.11
 * @author Ilia Vaprol
 */

#include <time.h>
#include <kernel/timer.h>


useconds_t clock(void) {
	return (useconds_t) clock_to_usec(cnt_system_time(), clock_source);
}

struct timeval * get_timeval(struct timeval *now) {
	useconds_t usec = clock_to_usec(cnt_system_time(), clock_source);
	now->tv_sec = usec / CLOCKS_PER_SEC;
	now->tv_usec = usec % CLOCKS_PER_SEC;
	return now;
}

time_t time(time_t *now) {
	if (now) {
		*now = clock_to_usec(cnt_system_time(), clock_source) / CLOCKS_PER_SEC;
		return *now;
	}
	return (time_t) (cnt_system_time() / CLOCKS_PER_SEC);
}

useconds_t clock_to_usec(clock_t now, uint32_t clock_src) {
	return (now * CLOCKS_PER_SEC) / clock_src;
}

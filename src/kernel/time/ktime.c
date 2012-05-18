/**
 * @file
 *
 * @brief
 *
 * @date 18.05.2012
 * @author Anton Bondarev
 */
#include <time.h>
#include <kernel/ktime.h>

static useconds_t time_usec(void) {
	return clock_source_clock_to_usec(clock_source_get_default(), clock());
}

struct ktimeval * get_timeval(struct ktimeval *tv) {
	useconds_t usec;

	usec = time_usec();
	tv->tv_sec = usec / USEC_PER_SEC;
	tv->tv_usec = usec % USEC_PER_SEC;
	return tv;
}

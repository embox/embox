/**
 * @file
 *
 * @date 12.12.12
 * @author Alexander Kalmuk
 */

#include <time.h>
#include <kernel/time/time.h>

/*Note: settimeofday is not posix */
int gettimeofday(struct timeval *t, struct timezone *tz) {
	struct timespec ts;

	if (t == NULL) {
		return -1;
	}

	getnsofday(&ts, tz);
	t->tv_sec = ts.tv_sec;
	t->tv_usec = ts.tv_nsec / 1000;

	return 0;
}

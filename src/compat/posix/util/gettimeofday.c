/**
 * @file
 *
 * @date 12.12.12
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <stddef.h>
#include <time.h>
#include <sys/time.h>
#include <kernel/time/time.h>

int gettimeofday(struct timeval *t, void *timezone) {
	struct timezone *tz;
	struct timespec ts;

	tz = timezone;

	if (t == NULL) {
		SET_ERRNO(EINVAL);
		return -1;
	}

	getnsofday(&ts, tz);
	t->tv_sec = ts.tv_sec;
	t->tv_usec = ts.tv_nsec / NSEC_PER_USEC;

	return 0;
}

/*Note: settimeofday is not posix */
int settimeofday(const struct timeval *tv, const struct timezone *tz) {
	struct timespec ts;

	if (tv == NULL) {
		SET_ERRNO(EINVAL);
		return -1;
	}
	ts.tv_sec = tv->tv_sec;
	ts.tv_nsec = tv->tv_usec * NSEC_PER_USEC;

	setnsofday(&ts, tz);

	return 0;
}

/**
 * @file
 *
 * @date Sep 25, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <time.h>

#include <kernel/time/ktime.h>

int nanosleep(const struct timespec *rqtp, struct timespec *rmtp) {
	int res;
	time64_t nsec;
	struct timespec start_time;

	clock_gettime(0, &start_time);

	timespec_to_ns(rqtp);

	res = ksleep(nsec * 1000);
	if (res < 0) {
		if (NULL != rmtp) {
			clock_gettime(0, rmtp);
			*rmtp = timespec_sub(start_time, *rmtp);
		}
		SET_ERRNO(EINTR);
		return -1;
	}
	return ENOERR;
}

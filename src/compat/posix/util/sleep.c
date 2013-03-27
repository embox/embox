/**
 * @file
 *
 * @date 12.12.12
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <unistd.h>
#include <kernel/time/ktime.h>
#include <sys/types.h>
#include <time.h>

int nanosleep(const struct timespec *req, struct timespec *rem) {
	int res;

	//TODO it must contains remind time if nanosleep was broken by a signal
	rem = NULL;

	res = sleep(req->tv_sec);
	if (res < 0) {
		SET_ERRNO(-res);
		return -1;
	}
	res = n_ksleep(req->tv_nsec);
	if (res < 0) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
}

int usleep(useconds_t usec) {
	int res;

	res = u_ksleep(usec);
	if (res < 0) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
}

int msleep(useconds_t msec) {
	int res;

	res = m_ksleep(msec);
	if (res < 0) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
}

int sleep(unsigned int seconds) {
	return msleep(seconds * 1000);
}

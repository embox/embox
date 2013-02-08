/**
 * @file
 *
 * @date 12.12.12
 * @author Alexander Kalmuk
 */

#include <errno.h>
#include <types.h>
#include <unistd.h>
#include <kernel/time/ktime.h>

int nsleep(useconds_t nsec) {
	int res;

	res = n_ksleep(nsec);
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

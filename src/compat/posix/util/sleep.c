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

int usleep(useconds_t usec) {
	int res;

	res = ksleep(usec);
	if (res < 0) {
		SET_ERRNO(-res);
		return -1;
	}

	return res;
}

int sleep(unsigned int seconds) {
	return usleep(seconds * 1000);
}

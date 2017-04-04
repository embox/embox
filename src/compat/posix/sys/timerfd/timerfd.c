/**
 * @file
 * @brief Timers that notify via file descriptors.
 * @details Right now this is a stub.
 *
 * @date 29.03.17
 * @author Kirill Smirenko
 */

#include <time.h>

int timerfd_create(int clockid, int flags) {
	return 0;
}

int timerfd_settime(int fd, int flags,
	const struct itimerspec *new_value,
	struct itimerspec *old_value) {
	return 42;
}

int timerfd_gettime(int fd, struct itimerspec *curr_value) {
	return 42;
}

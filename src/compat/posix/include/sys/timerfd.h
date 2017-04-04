/**
 * @file
 * @brief Timers that notify via file descriptors.
 *
 * @date 29.03.17
 * @author Kirill Smirenko
 */

#ifndef SYS_TIMERFD_H
#define SYS_TIMERFD_H

#include <time.h>

extern int timerfd_create(int clockid, int flags);

extern int timerfd_settime(int fd, int flags,
	const struct itimerspec *new_value,
	struct itimerspec *old_value);

extern int timerfd_gettime(int fd, struct itimerspec *curr_value);

#endif /* SYS_TIMERFD_H  */

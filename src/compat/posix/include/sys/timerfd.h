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

#include <sys/cdefs.h>

/* Bits to be set in the FLAGS parameter of `timerfd_create'.  */
enum {
	TFD_CLOEXEC = 02000000,
#define TFD_CLOEXEC TFD_CLOEXEC
	TFD_NONBLOCK = 04000
#define TFD_NONBLOCK TFD_NONBLOCK
};


/* Bits to be set in the FLAGS parameter of `timerfd_settime'.  */
#define TFD_TIMER_ABSTIME (1 << 0)
#define TFD_TIMER_CANCEL_ON_SET (1 << 1)
#define TFD_SETTIME_FLAGS (TFD_TIMER_ABSTIME | TFD_TIMER_CANCEL_ON_SET)

__BEGIN_DECLS

extern int timerfd_create(int clockid, int flags);

extern int timerfd_settime(int fd, int flags,
	const struct itimerspec *new_value,
	struct itimerspec *old_value);

extern int timerfd_gettime(int fd, struct itimerspec *curr_value);

__END_DECLS

#endif /* SYS_TIMERFD_H  */

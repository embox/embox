/*
 * @brief Kernel time
 *
 * @date 13.06.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TIME_KTIME_H_
#define KERNEL_TIME_KTIME_H_

#include <kernel/clock_source.h>

//move to posix
struct ktimespec {
	long tv_sec;    /* seconds */
	long tv_nsec;   /* nanoseconds */
};

//move to posix
struct ktimeval {
	long tv_sec;    /* seconds */
	long tv_usec;   /* microseconds */
};

extern struct ktimeval *ktime_get_timeval(struct ktimeval *tv);
extern struct ktimespec *ktime_get_timespec(struct ktimespec *ts);
extern ns_t ktime_get_ns(void);

/* Return active time source. */
extern struct clock_source *ktime_get_time_source(void);

#endif /* KERNEL_TIME_KTIME_H_ */

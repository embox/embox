/**
 * @file
 * @brief Kernel time
 *
 * @date 13.06.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TIME_KTIME_H_
#define KERNEL_TIME_KTIME_H_


#include <sys/types.h>
#include <kernel/time/time.h>

struct timeval;
struct timespec;

extern struct timeval *ktime_get_timeval(struct timeval *tv);
extern struct timespec *ktime_get_timespec(struct timespec *ts);
extern time64_t ktime_get_ns(void);
extern int ksleep(useconds_t usec);
extern time_t ktime_get_timeseconds(void);

#endif /* KERNEL_TIME_KTIME_H_ */

/**
 * @file
 * @brief Kernel time
 *
 * @date 13.06.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TIME_KTIME_H_
#define KERNEL_TIME_KTIME_H_

#include <kernel/time/clock_source.h>
#include <time.h>

extern struct timeval *ktime_get_timeval(struct timeval *tv);
extern struct timespec *ktime_get_timespec(struct timespec *ts);
extern time64_t ktime_get_ns(void);
extern int n_ksleep(useconds_t nsec);
extern int u_ksleep(useconds_t usec);
extern int m_ksleep(useconds_t msec);

#endif /* KERNEL_TIME_KTIME_H_ */

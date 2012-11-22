/**
 * @file
 * @brief
 *
 * @date 17.07.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TIME_TIME_H_
#define KERNEL_TIME_TIME_H_

#include <time.h>
#include <stdint.h>
#include <kernel/time/time_types.h>

/* gettimeofday is posix function, but settimeofday is not. */
extern void settimeofday(struct timespec *ts, struct timezone *tz);
extern void getnsofday(struct timespec *t, struct timezone *tz);

extern struct timespec timespec_add(struct timespec t1,
		struct timespec t2);
extern struct timespec timespec_sub(struct timespec t1,
		struct timespec t2);

static inline ns_t timespec_to_ns(const struct timespec *ts) {
	return ((__s64) ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

static inline ns_t timeval_to_ns(const struct timeval *tv) {
	return ((__s64) tv->tv_sec * NSEC_PER_SEC) + tv->tv_usec * NSEC_PER_USEC;
}

extern struct timespec ns_to_timespec(const __s64 nsec);
extern struct timeval ns_to_timeval(const __s64 nsec);

extern clock_t ns_to_clock(__s64 ns);

#endif /* KERNEL_TIME_TIME_H_ */

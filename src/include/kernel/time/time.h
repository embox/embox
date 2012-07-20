/**
 * @brief
 *
 * @date 17.07.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TIME_TIME_H_
#define KERNEL_TIME_TIME_H_

#include <time.h>
#include <stdint.h>

/* gettimeofday is posix function, but settimeofday is not. */
extern void settimeofday(struct timespec *ts, struct timezone *tz);

extern struct timespec timespec_add(struct timespec t1,
		struct timespec t2);
extern struct timespec timespec_sub(struct timespec t1,
		struct timespec t2);

#endif /* KERNEL_TIME_TIME_H_ */

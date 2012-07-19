/*
 * @brief
 *
 * @date 17.07.2012
 * @author Alexander Kalmuk
 */

#ifndef ABSTIME_H_
#define ABSTIME_H_

#include <time.h>

/* gettimeofday is posix function, but settimeofday is not. */
extern void settimeofday(struct timespec *ts, struct timezone *tz);

#endif /* ABSTIME_H_ */

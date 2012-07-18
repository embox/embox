/*
 * @brief
 *
 * @date 17.07.2012
 * @author Alexander Kalmuk
 */

#ifndef ABSTIME_H_
#define ABSTIME_H_

#include <time.h>

extern void time_update(struct timespec *ts);
extern void gettimeofday(struct timespec *ts);

#endif /* ABSTIME_H_ */

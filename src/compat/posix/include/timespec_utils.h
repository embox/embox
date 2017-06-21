/**
 * @file
 * @brief Timespec arithmetic utils.
 *
 * @date 21.06.17
 * @author Kirill Smirenko
 */

#ifndef TIMESPEC_UTILS_H
#define TIMESPEC_UTILS_H

#include <time.h>

extern int timespec_is_zero(const struct timespec *ts);

extern int timespec_is_positive(const struct timespec *ts);

extern int timespec_is_non_negative(const struct timespec *ts);

extern void timespec_inc_by(struct timespec *ts, const struct timespec *add);

extern void timespec_diff(const struct timespec *first, struct timespec *second,
		struct timespec *result);

#endif /* TIMESPEC_UTILS_H */

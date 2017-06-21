/**
 * @file
 * @brief Timespec arithmetic utils.
 *
 * @date 21.06.17
 * @author Kirill Smirenko
 */

#include <timespec_utils.h>

int timespec_is_zero(const struct timespec *ts) {
	return (ts->tv_sec == 0) && (ts->tv_nsec == 0);
}

int timespec_is_positive(const struct timespec *ts) {
	return (ts->tv_sec > 0) || (ts->tv_nsec > 0);
}

int timespec_is_non_negative(const struct timespec *ts) {
	if (ts->tv_sec < 0) {
		return 0;
	}
	if (ts->tv_nsec < 0) {
		return 0;
	}
	return 1;
}

void timespec_inc_by(struct timespec *ts, const struct timespec *add) {
	ts->tv_sec += add->tv_sec;
	ts->tv_nsec += add->tv_sec;
	if ((NSEC_PER_SEC - ts->tv_nsec) < 0) {
		ts->tv_nsec -= NSEC_PER_SEC;
		ts->tv_sec += 1;
	}
}

void timespec_diff(const struct timespec *first, struct timespec *second,
		struct timespec *result) {
	if ((second->tv_nsec - first->tv_nsec) >= 0) {
		result->tv_sec = second->tv_sec - first->tv_sec;
		result->tv_nsec = second->tv_nsec - first->tv_nsec;
	} else {
		result->tv_sec = second->tv_sec - first->tv_sec - 1;
		result->tv_nsec = second->tv_nsec - first->tv_nsec + NSEC_PER_SEC;
	}
}

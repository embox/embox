/**
 * @file
 * @brief
 *
 * @date 17.07.2012
 * @author Alexander Kalmuk
 */

#include <stdint.h>
#include <time.h>

#include <kernel/time/time.h>

/**
 * Assigns number of seconds and nanoseconds to a \ref timespec.
 *
 * Values are @a normalized before assigning: they are adjusted in a way
 * that the overall value they represent remains the same, but the number
 * of nanoseconds becomes more than zero and less than the number of
 * nanoseconds in second, i.e. seconds "included" in nanoseconds value
 * are "moved" from it to the seconds value.
 *
 * @param ts pointer to a timespec to assign values to
 * @param sec number of seconds
 * @param nsec number of nanoseconds
 */
static void set_normalized_timespec(struct timespec *ts, time_t sec,
    long nsec) {
	while (nsec >= NSEC_PER_SEC) {
		nsec -= NSEC_PER_SEC;
		sec++;
	}

	while (nsec < 0) {
		nsec += NSEC_PER_SEC;
		sec--;
	}

	ts->tv_sec = sec;
	ts->tv_nsec = nsec;
}

struct timespec timespec_add(struct timespec t1, struct timespec t2) {
	struct timespec ts;

	set_normalized_timespec(&ts, t1.tv_sec + t2.tv_sec,
	    t1.tv_nsec + t2.tv_nsec);

	return ts;
}

struct timespec timespec_sub(struct timespec t1, struct timespec t2) {
	struct timespec ts;

	set_normalized_timespec(&ts, t1.tv_sec - t2.tv_sec,
	    t1.tv_nsec - t2.tv_nsec);

	return ts;
}

#if 0
struct timespec timespec_add_ns(struct timespec t, time64_t ns) {
	struct timespec res;

	set_normalized_timespec(&res, t.tv_sec + ns / NSEC_PER_SEC, t.tv_nsec + ns % NSEC_PER_SEC);

	return res;
}
#endif

struct timespec ns_to_timespec(const int64_t nsec) {
	struct timespec ts;

	ts.tv_sec = nsec / NSEC_PER_SEC;
	ts.tv_nsec = nsec % NSEC_PER_SEC;
	/* and normalize result */
	if (nsec < 0) {
		ts.tv_sec--;
		ts.tv_nsec += NSEC_PER_SEC;
	}

	return ts;
}

struct timeval ns_to_timeval(const int64_t nsec) {
	struct timeval tv;
	struct timespec ts = ns_to_timespec(nsec);

	tv.tv_sec = ts.tv_sec;
	tv.tv_usec = ts.tv_nsec / NSEC_PER_USEC;

	return tv;
}

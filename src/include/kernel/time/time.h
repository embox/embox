/**
 * @file
 * @brief Utils for conversion beetween time types
 *
 * @date 17.07.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_TIME_TIME_H_
#define KERNEL_TIME_TIME_H_

#include <assert.h>
#include <stdint.h>
#include <sys/cdefs.h>
#include <sys/time.h>
#include <time.h>

#include <framework/mod/options.h>

#include <config/embox/kernel/time/jiffies.h>

#define MSEC_PER_SEC  1000L
#define USEC_PER_MSEC 1000L
#define USEC_PER_SEC  1000000L
#define NSEC_PER_USEC 1000L
#define NSEC_PER_MSEC 1000000L
#define NSEC_PER_SEC  1000000000L
#define SEC_PER_DAY   86400L

#define JIFFIES_PERIOD \
	OPTION_MODULE_GET(embox__kernel__time__jiffies, NUMBER, period)

/**
 * Storage for hardware internal cycles
 */
typedef uint64_t cycle_t;
/**
 * There are to ways to store time in kernel:
 * 1. Throw timeval and timespec.
 * 2. Throw int64_t.
 * This two approaches are equal, but int64_t is faster for arithmetic
 * operations. So, we use second.
 * We use signed integer to simplify subtraction.
 * This types can store about 584 years in nanoseconds
 * (assuming CPU frequency is about 1 GHz).
 */
typedef int64_t time64_t;

__BEGIN_DECLS

/* gettimeofday is posix function, but settimeofday is not. */
extern void setnsofday(const struct timespec *ts, const struct timezone *tz);
extern void getnsofday(struct timespec *t, struct timezone *tz);

/**
 * Returns a new \ref timespec representing the time given as input
 *
 * @param nsec a time in nanoseconds to be converted to timespec
*/
extern struct timespec ns_to_timespec(const int64_t nsec);
/**
 * Returns a new \ref timeval representing the time given as input
 *
 * @param nsec a time in nanoseconds to be converted to timeval
*/
extern struct timeval ns_to_timeval(const int64_t nsec);

/**
 * Returns a new \ref timespec representing the result of summing up
 * given timespecs
 */
extern struct timespec timespec_add(struct timespec t1, struct timespec t2);
/**
 * Returns a new \ref timespec representing the result of subtraction
 * of two given timespecs.
 *
 * @param t1 timespec to subtract from
 * @param t2 timespec to be subtracted
 */
extern struct timespec timespec_sub(struct timespec t1, struct timespec t2);
#if 0
extern struct timespec timespec_add_ns(struct timespec t, time64_t ns);
#endif

static inline time64_t timespec_to_ns(const struct timespec *ts) {
	assert(ts != NULL);
	return ((int64_t)ts->tv_sec * NSEC_PER_SEC) + ts->tv_nsec;
}

static inline time64_t timespec_to_ms(const struct timespec *ts) {
	assert(ts != NULL);
	return ((int64_t)ts->tv_sec * MSEC_PER_SEC) + ts->tv_nsec / NSEC_PER_MSEC;
}

static inline uint64_t timespec_to_hw(const struct timespec *ts, uint32_t hz) {
	return (uint64_t)ts->tv_sec * hz
	       + ((uint64_t)ts->tv_nsec * hz) / NSEC_PER_SEC;
}

static inline unsigned long timeval_to_ms(const struct timeval *tv) {
	assert(tv != NULL);
	return ((unsigned long)tv->tv_sec * MSEC_PER_SEC)
	       + tv->tv_usec / USEC_PER_MSEC;
}

static inline void ms_to_timeval(unsigned long ms, struct timeval *tv) {
	assert(tv != NULL);
	tv->tv_sec = ms / MSEC_PER_SEC;
	tv->tv_usec = (ms % MSEC_PER_SEC) * USEC_PER_MSEC;
}

static inline time64_t timeval_to_ns(const struct timeval *tv) {
	assert(tv != NULL);
	return ((int64_t)tv->tv_sec * NSEC_PER_SEC) + tv->tv_usec * NSEC_PER_USEC;
}

/* Round up */
static inline time64_t clock_to_ns(uint32_t hz, clock_t ticks) {
	assert(hz != 0);
	return ((time64_t)ticks * NSEC_PER_SEC + hz - 1) / hz;
}

static inline clock_t ns_to_clock(uint32_t hz, time64_t ns) {
	assert(hz != 0);
	/* for example, 10001000 ns = 1 ms 1 usec, hz = 1000. Then,
	 * there should be 3 clocks to guarantee 1 ms 1 usec fits the interval. */
	return (ns * hz + NSEC_PER_SEC - 1) / NSEC_PER_SEC + 1;
}

static inline struct timespec cycles64_to_timespec(uint32_t hz,
    uint64_t cycles) {
	struct timespec ts = {0, 0};
	assert(hz != 0);
	ts.tv_sec = cycles / hz;
	ts.tv_nsec = ((cycles % hz) * NSEC_PER_SEC) / hz;
	return ts;
}

static inline struct timespec cycles32_to_timespec(uint32_t cycles,
    uint32_t mult, uint32_t shift) {
	struct timespec ts = {0, 0};
	ts.tv_nsec = ((uint64_t)cycles * mult) >> shift;
	return ts;
}

static inline struct timespec jiffies_to_timespec(uint32_t hz,
    uint32_t jiffies) {
	struct timespec ts = {0, 0};
	assert(hz != 0);
	ts.tv_sec = jiffies / hz;
	ts.tv_nsec = (NSEC_PER_SEC / hz) * (jiffies % hz);
	return ts;
}

static inline cycle_t ns_to_cycles(uint32_t hz, time64_t ns) {
	assert(hz != 0);
	return (ns * hz + NSEC_PER_SEC - 1) / NSEC_PER_SEC;
}

extern clock_t ns2jiffies(time64_t ns);
extern clock_t ms2jiffies(time64_t ms);
extern time64_t jiffies2ms(clock_t jiff);
extern int jiffies_init(void);
extern void jiffies_update(int ticks);

extern int monotonic_clock_select(void);
extern int realtime_clock_select(void);

__END_DECLS

#endif /* KERNEL_TIME_TIME_H_ */

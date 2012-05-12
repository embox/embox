/*
 * @file Time format conversion
 *
 * @date 11.05.2012
 * @author Alexander Kalmuk
 */

#ifndef KERNEL_KTIME_H_
#define KERNEL_KTIME_H_

#include <stdint.h>
#include <kernel/clock_source.h>

/* Parameters used to convert the time specific values */
#define MSEC_PER_SEC    1000L
#define USEC_PER_MSEC   1000L
#define NSEC_PER_USEC   1000L
#define USEC_PER_SEC    1000000L
#define NSEC_PER_SEC    1000000000L

/* nanosecond-resolution time format */
typedef uint64_t ns_t;

/*
* Hardware abstraction for a free running counter.
* @param read  - returns the current cycle value
* @param mult  - cycle to nanosecond multiplier
* @param shift - cycle to nanosecond divisor (power of two)
*/
struct cyclecounter {
	cycle_t (*read)(const struct cyclecounter *cc);
#if 0
	cycle_t mask;
#endif
	uint32_t mult;
	uint32_t shift;
};

/**
 * Layer above a cyclecounter which counts nanoseconds.
 * @param cc         - the cycle counter used by this instance
 * @param cycle_last - most recent cycle counter value seen by
 *                    timecounter_read()
 * @param nsec       - continuously increasing count
 */
struct timecounter {
	const struct cyclecounter *cc;
	cycle_t cycle_last;
	uint64_t nsec;
};

static inline ns_t cycles_to_ns(const struct cyclecounter *cc, cycle_t cycles) {
	return (cycles * cc->mult) >> cc->shift;
}

#endif /* KERNEL_KTIME_H_ */

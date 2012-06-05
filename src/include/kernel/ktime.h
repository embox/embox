/*
 * @file Kernel time
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

struct clock_source;
/**
 * Layer above a cyclecounter which counts nanoseconds.
 * @param cc         - the cycle counter used by this instance
 * @param cycle_last - most recent cycle counter value seen by
 *                    timecounter_read()
 * @param nsec       - continuously increasing count
 */
struct timecounter {
//	const struct cyclecounter *cc;
//	/* pointer to time device assigned to timecounter */
//	const struct clock_event_device *dev;
	struct clock_source *cs;
	uint32_t jiffies_last;
	cycle_t cycle_last;
	ns_t nsec;
};

static inline ns_t cycles_to_ns(const struct cyclecounter *cc, cycle_t cycles) {
	return (cycles * cc->mult) >> cc->shift;
}

struct ktimespec {
	long tv_sec;    /* seconds */
	long tv_nsec;   /* nanoseconds */
};


struct ktimeval {
	long tv_sec;    /* seconds */
	long tv_usec;   /* microseconds */
};

/* time in struct timeval from start of system */
extern struct ktimeval * ktime_get_timeval(struct ktimeval *);

extern ns_t ktime_get_ns(void);


#endif /* KERNEL_KTIME_H_ */

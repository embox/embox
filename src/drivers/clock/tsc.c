/**
 * @file
 * @brief Time Stamp Counter
 *
 * @date 27.06.2012
 * @author Alexander Kalmuk
 */

#include <types.h>
#include <errno.h>
#include <util/array.h>

#include <kernel/clock_source.h>
#include <kernel/clock_event.h>
#include <kernel/time/ktime.h>

/* configurable */
#define TSC_HZ 1000000000L

static int tsc_init(void);

static inline unsigned long long rdtsc(void) {
  unsigned hi, lo;
  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
  return ( (unsigned long long)lo) | ( ((unsigned long long)hi) << 32 );
}

static struct time_counter_device tsc = {
	.init = tsc_init,
	.read = rdtsc,
	.resolution = TSC_HZ
};

static struct clock_source tsc_clock_source = {
	.name = "TSC",
	.event_device = NULL,
	.counter_device = &tsc,
	.read = clock_source_counter_read /* attach default read function */
};

CLOCK_SOURCE(&tsc_clock_source);

static int tsc_init(void) {
	/* TODO get CPU hz */
	tsc.resolution = TSC_HZ;
	return ENOERR;
}

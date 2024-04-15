/**
 * @file
 * @brief Time Stamp Counter
 *
 * @date 27.06.2012
 * @author Alexander Kalmuk
 */

#include <stdint.h>
#include <errno.h>
#include <lib/libds/array.h>
#include <unistd.h>
#include <stdio.h>

#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>
#include <hal/system.h>

#include <embox/unit.h>


/* Read Time Stamp Counter Register */
static inline unsigned long long rdtsc(struct clock_source *cs) {
	unsigned hi, lo;
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return (((unsigned long long) lo) | (((unsigned long long) hi) << 32));
}

static struct time_counter_device tsc = {
	.read = rdtsc
};

static int tsc_init(struct clock_source *cs) {
	time64_t t1, t2;
	/* Getting CPU frequency */
	t1 = rdtsc(cs);
	sleep(1);
	t2 = rdtsc(cs);
	tsc.cycle_hz = t2 - t1;

	return ENOERR;
}

CLOCK_SOURCE_DEF(tsc, tsc_init, NULL,
	NULL, &tsc);

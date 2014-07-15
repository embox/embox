/**
 * @file
 * @brief Time Stamp Counter
 *
 * @date 27.06.2012
 * @author Alexander Kalmuk
 */

#include <stdint.h>
#include <errno.h>
#include <util/array.h>
#include <unistd.h>
#include <stdio.h>

#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>
#include <hal/system.h>

#include <embox/unit.h>

static int tsc_init(void);

/* Read Time Stamp Counter Register */
static inline unsigned long long rdtsc(void) {
	unsigned hi, lo;
	__asm__ __volatile__("rdtsc" : "=a"(lo), "=d"(hi));
	return (((unsigned long long) lo) | (((unsigned long long) hi) << 32));
}

static struct time_counter_device tsc = {
	.read = rdtsc
};

static struct clock_source tsc_clock_source = {
	.name = "TSC",
	.event_device = NULL,
	.counter_device = &tsc,
	.read = clock_source_read /* attach default read function */
};

static int tsc_init(void) {
	time64_t t1, t2;
	/* Getting CPU frequency */
	t1 = rdtsc();
	sleep(1);
	t2 = rdtsc();
	tsc.cycle_hz = t2 - t1;
	/*printk("CPU frequency: %llu\n", t2 - t1);*/
	clock_source_register(&tsc_clock_source);
	return ENOERR;
}

EMBOX_UNIT_INIT(tsc_init);

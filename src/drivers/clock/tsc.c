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
#include <asm/regs.h>

#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>

#include <embox/unit.h>

static int tsc_init(void);

static unsigned int cpu_hz = 1000000000L;

static struct time_counter_device tsc = {
	.read = rdtsc
};

static struct clock_source tsc_clock_source = {
	.name = "TSC",
	.event_device = NULL,
	.counter_device = &tsc,
	.read = clock_source_counter_read /* attach default read function */
};

static int tsc_init(void) {
	/* TODO get CPU hz */
	tsc.resolution = cpu_hz;
	clock_source_register(&tsc_clock_source);
	return ENOERR;
}

EMBOX_UNIT_INIT(tsc_init);

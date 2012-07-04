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

#include <kernel/clock_source.h>
#include <kernel/clock_event.h>
#include <kernel/time/ktime.h>

static int tsc_init(void);

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

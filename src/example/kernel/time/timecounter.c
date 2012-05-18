/**
 * @file
 *
 * @brief
 *
 * @date 15.05.2012
 * @author Anton Bondarev
 */
#include <stdio.h>
#include <errno.h>
#include <framework/example/self.h>

#include <kernel/clock_source.h>
#include <kernel/clock_event.h>
#include <kernel/ktime.h>
#include <kernel/time/timecounter.h>

EMBOX_EXAMPLE(run);

static const struct clock_event_device *dev;

static int measured_loop(int cycles_loop) {
	volatile int i;
	ns_t time_after;
	struct timecounter tc;

	timecounter_init(&tc, dev->cs->cc, 0);
	for (i = 0; i < cycles_loop; i++) {
	}
	time_after = timecounter_read(&tc);
	printf("spent = %d\n", (int) time_after);

	return 0;
}

static int run(int argc, char **argv) {
	dev = cedev_get_by_name("pit");
	measured_loop(10000);
	measured_loop(1000);
	measured_loop(100);
	return ENOERR;
}

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

#include <kernel/time/clock_source.h>
#include <kernel/time/ktime.h>
#include <kernel/time/itimer.h>

EMBOX_EXAMPLE(run);

static struct clock_source *cs;

static int measured_loop(int cycles_loop) {
	volatile int i;
	time64_t time_after;
	struct itimer it;

	itimer_init(&it, cs, 0);
	for (i = 0; i < cycles_loop; i++) {
	}
	time_after = itimer_read(&it);
	printf("spent = %d\n", (int) time_after);

	return 0;
}

static int run(int argc, char **argv) {
	cs = clock_source_get_best(CS_ANY);

	for (int i = 0; i < 100; i++) {
		measured_loop(1000);
	}

	return ENOERR;
}

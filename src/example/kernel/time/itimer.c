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

static int measured_loop(struct clock_source *cs, int cycles_loop) {
	volatile int i;
	time64_t time_after;
	struct itimer it;

	itimer_init(&it, cs, 0);
	for (i = 0; i < cycles_loop; i++) {
	}
	time_after = itimer_read(&it);
	printf("%d ", (int) time_after);

	return 0;
}

static int run(int argc, char **argv) {
	struct clock_source_head *csh;
	struct clock_source *cs;

	clock_source_foreach(csh) {
		cs = csh->clock_source;

		printf("%s: ", cs->name);

		for (int i = 0; i < 100; i++) {
			measured_loop(cs, 1000);
		}

		printf("\n\n");
	}

	return ENOERR;
}

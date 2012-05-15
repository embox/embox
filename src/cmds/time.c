/**
 * @file
 *
 * @brief
 *
 * @date 12.05.2012
 * @author Anton Bondatev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>

#include <util/array.h>
#include <framework/cmd/api.h>
#include <kernel/clock_source.h>
#include <kernel/ktime.h>
#include <kernel/time/timecounter.h>


EMBOX_CMD(exec);
static int measured_loop(int cycles_loop) {
	volatile int i;
	ns_t time_after;
	struct timecounter tc;


	timecounter_init(&tc, clock_source_get_default()->cc, 0);
	for(i = 0; i < cycles_loop; i ++) {
	}
	time_after = timecounter_read(&tc);
	printf("spent = %d\n", (int)time_after);

	return 0;
}

static int exec(int argc, char **argv) {
//	const struct cmd *cmd;

//	if (argc <= 1) {
//		printf("Please enter command name\n");
//	}

	//cmd = cmd_lookup(argc - 1);



	measured_loop(10000);
	measured_loop(1000);
	measured_loop(100);

	return 0;
}

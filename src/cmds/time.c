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

EMBOX_CMD(exec);

static int exec(int argc, char **argv) {
//	const struct cmd *cmd;

//	if (argc <= 1) {
//		printf("Please enter command name\n");
//	}

	//cmd = cmd_lookup(argc - 1);
#if 0
	volatile int i;
	cycle_t tics_before, tics_after;

	tics_before = i8253_read();
	for(i = 0; i < 10000; i ++) {

	}
	tics_after = i8253_read();
	printf("before = %d after = %d spent = %d\n", (int)tics_before, (int)tics_after, (int)(tics_after - tics_before));

	tics_before = i8253_read();
	for(i = 0; i < 1000; i ++) {

	}
	tics_after = i8253_read();
	printf("before = %d after = %d spent = %d\n", (int)tics_before, (int)tics_after, (int)(tics_after - tics_before));

	tics_before = i8253_read();
	for(i = 0; i < 100; i ++) {

	}
	tics_after = i8253_read();
	printf("before = %d after = %d spent = %d\n", (int)tics_before, (int)tics_after, (int)(tics_after - tics_before));
#endif
	return 0;
}

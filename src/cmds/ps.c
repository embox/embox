/**
 * @file
 * @brief
 *
 * @date 25.02.2012
 * @author Anton Bulychev
 */

#include <embox/cmd.h>

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <kernel/task/task_table.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: ps [-a]\n");
}

static void print_all(void) {
	int tid = 0;

	while (0 <= (tid = task_table_get_first(tid))) {
		printf("tid: %d\n", tid);
		++tid;
	}

}

static int exec(int argc, char **argv) {
	int opt;

	if (argc <= 1) {
		print_usage();
		return -1;
	}

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "ah:"))) {
		printf("\n");
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			break;
		case 'a':
			print_all();
			break;
		default:
			break;
		}
	}

	return 0;
}

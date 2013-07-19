/**
 * @file
 * @brief
 *
 * @date 25.02.2012
 * @author Anton Bulychev
 */

#include <embox/cmd.h>

#include <errno.h>
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
	struct task *task;

	printf(" %3s  %20s\n", "tid", "name");

	while (0 <= (tid = task_table_get_first(tid))) {
		task = task_table_get(tid);
		printf(" %3d  %20s\n", tid, task->task_name);

		++tid;
	}

}

static int exec(int argc, char **argv) {
	int opt;

	if (argc <= 1) {
		print_usage();
		return -EINVAL;
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

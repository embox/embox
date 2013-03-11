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
#include <stdlib.h>

#include <kernel/task.h>
#include <kernel/task/task_table.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: taskset -p [mask] tid\n");
}

static int exec(int argc, char **argv) {
	int opt;
	int tid;
	struct task *task;

	if (argc <= 1) {
		print_usage();
		return -EINVAL;
	}

	getopt_init();

	while (-1 != (opt = getopt(1, argv, "ph"))) {
		printf("\n");
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			return ENOERR;
		case 'p':
			break;
		default:
			return -EINVAL;
		}
	}

	tid = atoi(argv[argc - 1]);
	task = task_table_get(tid);

	if (task == NULL) {
		return -EINVAL;
	}

	printf("tid %d's current affinity mask: %x\n", tid, (~task->naffinity));

	if (argc == 4) {
		task->naffinity = ~(strtol(argv[argc - 2], NULL, 16));
		printf("tid %d's new affinity mask: %x\n", tid, (~task->naffinity));
	}

	return 0;
}

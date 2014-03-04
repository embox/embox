/**
 * @file
 * @brief
 *
 * @date 11.03.2013
 * @author Anton Bulychev
 */

#include <embox/cmd.h>

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/task.h>
#include <kernel/task/kernel_task.h>
#include <kernel/task/resource/affinity.h>
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

	while (-1 != (opt = getopt(argc, argv, "ph"))) {
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			return ENOERR;
		case 'p':
			tid = atoi(argv[argc - 1]);
			task = task_table_get(tid);

			if (task == NULL) {
				return -ENOENT;
			}

			printf("tid %d's current affinity mask: %x\n",
					tid, task_get_affinity(task));

			if (argc == 4) {
				if (task == task_kernel_task()) {
					printf("Couldn't change affinity mask of kernel task\n");
					return -EINVAL;
				} else {
					task_set_affinity(task, strtol(argv[argc - 2], NULL, 16));
					printf("tid %d's new affinity mask: %x\n",
						tid, task_get_affinity(task));
				}
			}

			return ENOERR;
		default:
			print_usage();
			return -EINVAL;
		}
	}

	print_usage();
	return -EINVAL;
}

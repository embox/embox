/**
 * @file
 * @brief
 *
 * @date 25.02.2012
 * @author Anton Bulychev
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <kernel/task.h>

static void print_usage(void) {
	printf("Usage: ps [-a]\n");
}


static void print_all(void) {
	struct task *task;

	printf(" %3s  %20s\n", "tid", "name");

	task_foreach(task) {
		printf(" %3d  %20s\n", task_get_id(task),
				task_get_name(task));
	}
}

int main(int argc, char **argv) {
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

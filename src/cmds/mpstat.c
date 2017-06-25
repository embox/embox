/**
 * @file
 * @brief
 *
 * @date 11.03.2013
 * @author Anton Bulychev
 */

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <hal/cpu.h>
#include <kernel/cpu/cpu.h>

static void print_usage(void) {
	printf("Usage: mpstat -P ALL\n");
}

int main(int argc, char **argv) {
	int opt;
	clock_t atotal = 0;
	clock_t aidle = 0;

	if (argc <= 1) {
		print_usage();
		return -EINVAL;
	}

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "Ph"))) {
		switch (opt) {
		case '?':
			printf("Invalid command line option\n");
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			return ENOERR;
		case 'P':
			printf("CPU  time  %%idle\n");

			for (int i = 0; i < NCPU; i++) {
				clock_t idle = cpu_get_idle_time(i);
				clock_t total = cpu_get_total_time(i);

				printf("%3d  %3ds    %2d%%\n",
						i,
						(int) (total / CLOCKS_PER_SEC),
						(int) (idle * 100 / total));

				atotal += total;
				aidle  += idle;
			}

			printf("ALL  %3ds    %2d%%\n",
					(int) (atotal / CLOCKS_PER_SEC),
					(int) (aidle * 100 / atotal));

			return ENOERR;
		default:
			print_usage();
			return -EINVAL;
		}
	}

	print_usage();
	return -EINVAL;
}

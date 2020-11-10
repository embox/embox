/**
 * @file
 *
 * @brief
 *
 * @date 12.05.2012
 * @author Anton Bondatev
 */

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int main(int argc, char **argv) {
	struct timespec ts_before, ts_after, ts_res;

	if (argc <= 1) {
		printf("Please enter command name\n");
		return -EINVAL;
	}

	clock_gettime(CLOCK_MONOTONIC, &ts_before);
	system(argv[1]);
	clock_gettime(CLOCK_MONOTONIC, &ts_after);
	timespecsub(&ts_after, &ts_before, &ts_res);

	printf("\ntime: %d.%03dS\n", (int)ts_res.tv_sec, (int)(ts_res.tv_nsec/1000000L));

	return 0;
}

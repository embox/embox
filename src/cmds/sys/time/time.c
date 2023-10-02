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
#include <string.h>

int main(int argc, char **argv) {
	struct timespec ts_before, ts_after, ts_res;
	char concat[128] = "";

	if (argc <= 1) {
		printf("Please enter command name\n");
		return -EINVAL;
	}

	for (int i = 1; i < argc; i++) {
		strncat(concat, argv[i], sizeof(concat) - 1);
		strncat(concat, " ", sizeof(concat) - 1);
		if (strlen(concat) >= (sizeof(concat) - 1)) {
			printf("Please use shorter command line\n");
			return -EINVAL;
		}

	}

	clock_gettime(CLOCK_MONOTONIC, &ts_before);
	system(concat);
	clock_gettime(CLOCK_MONOTONIC, &ts_after);
	timespecsub(&ts_after, &ts_before, &ts_res);

	printf("\ntime: %d.%03dS\n", (int) ts_res.tv_sec,
			(int) (ts_res.tv_nsec / 1000000L));

	return 0;
}

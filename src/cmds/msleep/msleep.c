/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    30.04.2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

static void usage(char *arg0) {
	printf("usage: %s MSEC\n", arg0);
}

int main(int argc, char **argv) {
	int msec = 0;

	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}

	if (0 > (msec = atoi(argv[1]))) {
		usage(argv[0]);
		return 1;
	}

	if (0 > usleep(1000 * msec)) {
		printf("%s: sleep error occured", argv[1]);
		return 2;
	}

	return 0;
}

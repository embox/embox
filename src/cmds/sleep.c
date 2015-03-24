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
	printf("usage: %s USEC\n", arg0);
}

int main(int argc, char **argv) {
	useconds_t usec = 0;

	if (argc != 2) {
		usage(argv[0]);
		return 1;
	}

	if (0 > (usec = atoi(argv[1]))) {
		usage(argv[0]);
		return 1;
	}

	if (0 > usleep(1000 * usec)) {
		printf("%s: sleep error occured", argv[1]);
		return 2;
	}

	return 0;
}

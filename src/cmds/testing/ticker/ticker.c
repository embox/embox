/**
 * @file
 *
 * @date Jun 7, 2018
 * @author Anton Bondarev
 */
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void print_usage(void) {
	printf("Usage: ticker [-h] [-c count] [-i interval sec]\n");
}

int main(int argc, char **argv) {
	int opt, i, step = 0;
	long count = 1, usec = USEC_PER_SEC;
	char *dot;

	while (-1 != (opt = getopt(argc, argv, "hc:i:"))) {
		switch (opt) {
		case 'c':
			count = strtol(optarg, NULL, 0);
			step = 1;
			break;
		case 'i':
			usec = strtol(optarg, NULL, 0) * USEC_PER_SEC;
			/* Try to parse float argument without actual use
			 * of FPU variables */
			if ((dot = strchr(optarg, '.'))) {
				long tmp = USEC_PER_SEC / 10;
				while (*(++dot)) {
					if (!isdigit(*dot)) {
						printf("Failed to parse "
						       "-i argument\n");

						return -EINVAL;
					}

					usec += tmp * (*dot - '0');
					tmp /= 10;
				}
			}
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}

	for (i = 1; i <= count || step == 0; i++) {
		if (usec % USEC_PER_SEC == 0) {
			printf("%ld sec\n", i * usec / USEC_PER_SEC);
		}
		else if (usec % MSEC_PER_SEC == 0) {
			printf("%ld msec\n", i * usec / MSEC_PER_SEC);
		}
		else {
			printf("%ld usec\n", i * usec);
		}

		usleep(usec);
	}

	return 0;
}

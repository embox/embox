/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    05.03.2013
 */

#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <embox/cmd.h>

EMBOX_CMD(kill_cmd);

static void print_usage(void) {
	printf("kill [-s SIGNUM] pid\n");
}

static int kill_cmd(int argc, char **argv) {
	int opt, sig = SIGKILL;
	int tid;

	getopt_init();

	while (-1 != (opt = getopt(argc, argv, "s:"))) {
		switch (opt) {
		case 's':
			sig = strtol(optarg, NULL, 0);
			break;
		default:
			printf("Unrecognized option: %c\n", opt);
			print_usage();
			return -EINVAL;
		}
	}

	if (sig <= 0) {
		printf("Invalid signal number: %d\n", sig);
		return -EINVAL;
	}

	if (optind == argc) {
		print_usage();
		return -EINVAL;
	}

	if (0 > (tid = strtol(argv[optind], NULL, 0))) {
		print_usage();
		return -EINVAL;
	}

	if (-1 == kill(tid, sig)) {
		return -errno;
	}

	return 0;

}

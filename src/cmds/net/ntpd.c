/**
 * @file
 * @brief SNTPv4 client
 *
 * @date 13.07.2012
 * @author Alexander Kalmuk
 */

#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <embox/cmd.h>
#include <getopt.h>

#include <kernel/task.h>
#include <net/ntp.h>
#include <net/in.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: ntpdate [-q] server");
}

static void *ntpd_run(void *arg) {
	if (ntp_start() == ENOERR)
		while(1);
	return NULL;
}

static int exec(int argc, char **argv) {
	int opt;
	struct in_addr sin_addr;

	getopt_init();
	while (-1 != (opt = getopt(argc - 1, argv, "h"))) {
		switch(opt) {
		case 'h':
			print_usage();
			return 0;
		default:
			break;
		}
	}

	if (!inet_aton(argv[argc - 1], &sin_addr)) {
		printf("Invalid ip address %s\n", argv[argc - 1]);
		return -ENOENT;
	}

	ntp_server_set(sin_addr.s_addr);

	if (argc == 2) {
		printf("*Starting NTP daemon\n");
		new_task(ntpd_run, NULL, 0);
	}

	return 0;
}

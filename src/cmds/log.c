/**
 * @file
 * @brief Ping hosts by ARP requests/replies.
 *
 * @date 23.12.09
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include <util/debug_msg.h>


EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: log [-c cnt]\n");
}

static int exec(int argc, char **argv) {
	int opt, cnt = 20;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "I:c:h"))) {
		switch (opt) {
		case 'c': /* get msg cnt */
			if (1 != sscanf(optarg, "%d", &cnt)) {
				printf("log: bad message count ot display.\n");
				return -1;
			}
			break;
		case '?':
			printf("Invalid option `-%c'\n", optopt);
			/* FALLTHROUGH */
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	};

	/* for(i=0; i<cnt; i++) */
	/* 	debug_printf("test", "log", "exec"); */
	output_debug_messages(cnt);

	return 0;
}

/**
 * @file
 * @brief
 *
 * @date 20.07.2012
 * @author Andrey Gazukin
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>
#include <drivers/ata.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: ide [OPTIONS]\n");
}

static int exec(int argc, char **argv) {
	uint32_t addr;
	int opt, addr_set;

	addr = 0;
	addr_set = 0;

	getopt_init();
	while (-1 != (opt = getopt(argc - 1, argv, "ah"))) {
		switch(opt) {
		case 'a':
			addr_set = 1;
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return -1;
		}
	}

	if(addr_set) {
		if (EOF == sscanf(argv[argc - 1], "%u", &addr)) {
			return -1;
		}
	}

	return detection_drive(addr);
}

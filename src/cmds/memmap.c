/**
 * @file
 *
 * @date Oct 8, 2012
 * @author: Anton Bondarev
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <stdio.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: memmap [-hsa]\n");
}

static void show_segments(void) {

}

static void show_all(void) {

}

static int exec(int argc, char **argv) {
	int opt;
	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "hkms"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 's':
			show_segments();
			return 0;
		case 'a':
			show_all();
			return 0;
		default:
			show_segments();
			return 0;
		}
	}
	return 0;
}

/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.05.2014
 */

#include <util/err.h>
#include <unistd.h>
#include <stdlib.h>
#include <drivers/ramdisk.h>

#define MKRAMDISK_DEFAULT_SIZE 8192;

int main(int argc, char *argv[]) {
	size_t ramdisk_size = MKRAMDISK_DEFAULT_SIZE;
	char *ramdisk_path = "/dev/ramdisk#";
	struct ramdisk *rd;
	int opt;

	while (-1 != (opt = getopt(argc, argv, "s:"))) {
		switch (opt) {
		case 's':
			ramdisk_size = strtol(optarg, NULL, 0);
			break;
		default:
			return 0;
		}
	}

	if (optind < argc) {
		ramdisk_path = argv[optind];
	}

	rd = ramdisk_create(ramdisk_path, ramdisk_size);
	return err(rd);
}

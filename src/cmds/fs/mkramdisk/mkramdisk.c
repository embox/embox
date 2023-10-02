/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    20.05.2014
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <drivers/block_dev.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>
#include <util/err.h>

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

	if (NULL != rd) {
		printf("%s\n", block_dev_name(rd->bdev));
	}

	return err(rd);
}

/**
 * @file
 * @brief Creates special file in virtual file systems
 *
 * @date 23.08.2013
 * @author Andrey Gazukin
 * @author Denis Deryugin
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <drivers/flash/emulator.h>

static void print_usage(void) {
	printf("Usage: mkflashemu [-h] [[-n BLOCKS] [-b BLOCK_SIZE] dev_name]\n");
}

int main(int argc, char **argv) {
	int opt;
	size_t block_size = 512;
	int blocks = 1024;

	while (-1 != (opt = getopt(argc - 1, argv, "hn:b:"))) {
		switch (opt) {
		case 'h':
			print_usage();
			return 0;
		case 'n':
			blocks = atol(optarg);
			break;
		case 'b':
			block_size = atol(optarg);
			break;
		default:
			break;
		}
	}

	if (argc != 2) {
		print_usage();
		return 0;
	}

	return flash_emu_dev_create(argv[argc - 1], blocks, block_size);
}

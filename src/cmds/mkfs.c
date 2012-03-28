/**
 * @file
 * @brief Manipulate the system ARP cache.
 *
 * @date 24.04.09
 * @author Nikolay Korotky
 */
#include <types.h>
#include <embox/cmd.h>
#include <fs/ramfs.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <fs/ramdisk.h>

#define MIN_ARGS_OF_MKFS 2;/** <mkfs /dev/ram0> must create ramdisk*/
#define DEFAULT_BLOCK_QTTY  0x04;
#define DEFAULT_FS_TYPE  "vfs";

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: mkfs  [ -t type ] file [ blocks ]\n");
}

static int exec(int argc, char **argv) {
	int opt;
	int min_argc;
	unsigned blocks;
	const char *fstype, *name;

	min_argc = MIN_ARGS_OF_MKFS;
	blocks = DEFAULT_BLOCK_QTTY;
	fstype = DEFAULT_FS_TYPE;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "ht:"))) {
		switch (opt) {
		case 't':
			min_argc = 4;
			fstype = optarg;
			break;
		case '?':
			if (min_argc > argc){
				printf("Invalid option `-%c' `%s'\n", optind, argv[optind]);
				print_usage();
				return -1;
			}
			break;
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
	}

	if (argc > min_argc){/** last arg should be block quantity*/
		if(0 >= sscanf(argv[argc - 1], "%d", &blocks)){
			print_usage();
			return -1;
		}
		name = argv[argc - 2];
	}
	else {/** last arg should be diskname*/
		name = argv[argc - 1];
	}

	return ramdisk_create(name, blocks, fstype);
}

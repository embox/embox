/**
 * @file mkfs.c
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#include <types.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fs/ramfs.h>
#include <fs/ramdisk.h>
#include <embox/cmd.h>
#include <embox/unit.h>
#include <util/array.h>
#include <mem/page.h>
#include <cmd/mkfs.h>


#define MIN_ARGS_OF_MKFS 2 /* <mkfs /dev/ram0> must create ramdisk*/
#define DEFAULT_BLOCK_QTTY  0x04
#define DEFAULT_FS_NAME  "vfat"
#define DEFAULT_FS_TYPE  16


EMBOX_CMD(exec);

static mkfs_params_t mkfs_params;

static void print_usage(void) {
	printf("Usage: mkfs  [ -t type ] file [ blocks ]\n");
}

static int check_invalid(int min_argc, int argc, char **argv) {
	if (min_argc > argc){
		printf("Invalid option `-%c' `%s'\n", optind, argv[optind]);
		print_usage();
		return 1;
	}
	else {
		return 0;
	}
}

static int exec(int argc, char **argv) {
	int opt;
	int min_argc;

	min_argc = MIN_ARGS_OF_MKFS;
	mkfs_params.blocks = DEFAULT_BLOCK_QTTY;

	strcpy((void *)&mkfs_params.fs_name, (const void *)DEFAULT_FS_NAME);

	mkfs_params.fs_type = DEFAULT_FS_TYPE;
	getopt_init();
	/* TODO add -q to create device, -F for vfat fstype 12/16/32 */
	while (-1 != (opt = getopt(argc, argv, "ht:"))) {
		switch (opt) {
		case 't':
			min_argc = 4;
			strcpy ((void *)&(mkfs_params.fs_name), (const void *)optarg);
			if(check_invalid(min_argc, argc, argv)){
				return -1;
			}
			break;
		case '?':
			if(check_invalid(min_argc, argc, argv)){
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

	if (argc > min_argc) {/** last arg should be block quantity*/
		if(0 >= sscanf(argv[argc - 1], "%d", &mkfs_params.blocks)){
			print_usage();
			return -1;
		}
		strcpy ((void *)&(mkfs_params.name), (const void *)argv[argc - 2]);
	}
	else {/** last arg should be diskname*/
		strcpy ((void *)&(mkfs_params.name), (const void *)argv[argc - 1]);

	}

	return ramdisk_create((void *)&mkfs_params);
}

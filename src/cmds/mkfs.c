/**
 * @file mkfs.c
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#include <types.h>
#include <errno.h>
#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fs/ramfs.h>
#include <fs/ramdisk.h>
#include <fs/fat.h>
#include <embox/cmd.h>
#include <embox/unit.h>
#include <util/array.h>
#include <mem/page.h>
#include <cmd/mkfs.h>


#define MIN_ARGS_OF_MKFS 2 /* <mkfs /dev/ram0> must create ramdisk*/
#define DEFAULT_BLOCK_QTTY  0x20
#define DEFAULT_FS_NAME  "vfat"
#define DEFAULT_FS_TYPE  16


EMBOX_CMD(exec);

static mkfs_params_t mkfs_params;
static ramdisk_params_t *ramd_params;

static void print_usage(void) {
	printf("Usage: mkfs  [ -t type ] file [ blocks ]\n");
}

static int check_invalid(int min_argc, int argc, char **argv) {
	if (min_argc > argc){
		printf("Invalid option `-%c' `%s'\n", optind, argv[optind]);
		print_usage();
		return EINVAL;
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
	mkfs_params.operation_flag = 0;

	strcpy((void *)&mkfs_params.fs_name, (const void *)DEFAULT_FS_NAME);

	mkfs_params.fs_type = DEFAULT_FS_TYPE;
	getopt_init();
	/* TODO add -q to create device, -F for vfat fstype 12/16/32 */
	while (-1 != (opt = getopt(argc, argv, "ht:q:"))) {
		switch (opt) {
		case 't':
			min_argc = 4;
			strcpy ((void *)&(mkfs_params.fs_name), (const void *)optarg);
			if(check_invalid(min_argc, argc, argv)){
				return -EINVAL;
			}
			mkfs_params.operation_flag |= MKFS_FORMAT_DEV;
			break;
		case 'q':
			min_argc += 1;
			mkfs_params.operation_flag |= MKFS_CREATE_DEV;
			if(check_invalid(min_argc, argc, argv)){
				return -EINVAL;
			}
			break;
		case '?':
			if(check_invalid(min_argc, argc, argv)){
				return -EINVAL;
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
			return -EINVAL;
		}
		strcpy ((void *)&(mkfs_params.name), (const void *)argv[argc - 2]);
	}
	else {/** last arg should be diskname*/
		strcpy ((void *)&(mkfs_params.name), (const void *)argv[argc - 1]);
	}

	return mkfs_do_operation(&mkfs_params);
}


int mkfs_do_operation(void *_mkfs_params) {
	mkfs_params_t *mkfs_params;
	fs_drv_t *fs_drv;
	int rezult;

	mkfs_params = (mkfs_params_t *) _mkfs_params;

	if(mkfs_params->operation_flag & MKFS_CREATE_DEV) {
		if(0 != (rezult = ramdisk_create((void *)mkfs_params))) {
			return rezult;
		}
#ifdef _GAZ_DEBUG_
		if(NULL == (ramd_params = ramdisk_get_param(mkfs_params->name))) {
			return -ENODEV;
		}

		printf("Create ramdisk %s, size %d, filesistem %s, addr %d,\n",
				ramd_params->name,ramd_params->size,
				ramd_params->fs_name, (int)ramd_params->start_addr);
#endif /*def _GAZ_DEBUG_ */
	}

	if(mkfs_params->operation_flag & MKFS_FORMAT_DEV) {
		if(NULL == (ramd_params = ramdisk_get_param(mkfs_params->name))) {
			return -ENODEV;
		}
		/* set filesystem attribute to ramdisk */
		strcpy ((void *)ramd_params->name, (const void *)mkfs_params->name);
		strcpy ((void *)ramd_params->fs_name,
					(const void *)mkfs_params->fs_name);
		ramd_params->fs_type = mkfs_params->fs_type;

		fs_drv = filesystem_find_drv((const char *) &mkfs_params->fs_name);

		/* format filesystem */
		if (0 != (rezult = fs_drv->fsop->create_file((void *)ramd_params))) {
			return rezult;/*file already exist*/
		}

#ifdef _GAZ_DEBUG_
		fat_main("/t1.txt");

		fat_main("/t2.txt");
#endif /*def _GAZ_DEBUG_ */
	}
	return 0;
}

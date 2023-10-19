/**
 * @file
 * @brief
 *
 * @date 27.03.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include <fs/fsop.h> /* mkfs() */

#include <fs/kfsop.h>
#include <fs/fs_driver.h>
#include <drivers/block_dev.h>

#include <util/err.h>

#define MKFS_CREATE_RAMDISK 0x00000001
#define MKFS_FORMAT_DEV     0x00000002

#define MIN_ARGS_OF_MKFS 3 /* <mkfs -q /dev/ram0> must create ramdisk*/
#define DEFAULT_BLOCK_QTTY  0x20
#define DEFAULT_FS_NAME  "vfat"
#define DEFAULT_FS_TYPE  12

#include <framework/mod/options.h>

#if OPTION_GET(NUMBER, with_ramdisk)

#include <drivers/block_dev/ramdisk/ramdisk.h>
#include <mem/page.h>

static int mkfs_create_ramdisk(char *path, int blocks) {
	return err(ramdisk_create(path, blocks * PAGE_SIZE()));
}
#else
static int mkfs_create_ramdisk(char *path, int blocks) {
	return -ENOTSUP;
}
#endif

#include <module/embox/fs/fs_api.h>

#ifdef __MODULE__embox__fs__core__H_
static int mkfs_do_operation(size_t blocks, char *path, const char *fs_name,
		unsigned int fs_type, unsigned int operation_flag, char *fs_specific) {
	int rezult;

	if (operation_flag & MKFS_CREATE_RAMDISK) {
		if (0 != (rezult = mkfs_create_ramdisk(path, blocks))) {
			return rezult;
		}
	}

	if (operation_flag & MKFS_FORMAT_DEV) {
		/* if (0 != (rezult = kformat(path, fs_name))) {
			return rezult;
		} */
	}
	return 0;
}
#elif defined __MODULE__embox__fs__dvfs__core__H_
	static int mkfs_do_operation(size_t blocks, char *path, const char *fs_name,
		int fs_type, int operation_flag, char *fs_specific) {
		const struct fs_driver *drv = fs_driver_find(fs_name);
		struct block_dev *bdev;
		struct lookup lu = {};
		int err;

		if (!drv) {
			printf("Unknown FS type: %s. Check your configuration.\n", fs_name);
			return 0;
		}

		if ((err = dvfs_lookup(path, &lu))) {
			return err;
		}

		if (!lu.item) {
			printf("File %s not found.\n", path);
			return 0;
		}

		assert(lu.item->d_inode);
		assert(lu.item->d_inode->i_data);

		bdev = dev_module_to_bdev(lu.item->d_inode->i_data);
		return drv->format(bdev, fs_specific);
	}
#endif

static void print_usage(void) {
	printf("Usage: mkfs [ -t type ] file [ blocks ]\n");
}

static int check_invalid(int min_argc, int argc, char **argv) {
	if (min_argc > argc){
		printf("Invalid option `-%c' `%s'\n", optind, argv[optind]);
		print_usage();
		return EINVAL;
	} else {
		return 0;
	}
}

int main(int argc, char **argv) {
	int opt;
	int min_argc;
	unsigned int operation_flag;
	int       blocks;
	char         *path;
	const char   *fs_name;
	unsigned int fs_type;
	char *fs_specific = NULL;

	min_argc = MIN_ARGS_OF_MKFS;

	operation_flag = 0;
	fs_name = DEFAULT_FS_NAME;
	fs_type = DEFAULT_FS_TYPE;
	blocks = DEFAULT_BLOCK_QTTY;

	if (argc < 2) {
		print_usage();
		return 0;
	}

	while (-1 != (opt = getopt(argc, argv, "ht:q:F:"))) {
		switch (opt) {
		case 'F':
			fs_specific = optarg;
			break;
		case 't':
			min_argc = 4;
			fs_name = optarg;
			operation_flag |= MKFS_FORMAT_DEV;
			break;
		case 'q':
			if (argc < 4) {
				/*mkfs -q /dev/xxx blocks*/
				return -EINVAL;
			}

			path = argv[argc - 2];

			if (0 >= sscanf(argv[argc - 1], "%d", &blocks)){
				print_usage();
				return -EINVAL;
			}

			return mkfs_create_ramdisk(path, blocks);
		case 'h':
			print_usage();
			return 0;
		default:
			return 0;
		}
		if (check_invalid(min_argc, argc, argv)) {
			return -EINVAL;
		}
	}

	if (argc > 1) {
		if (argc > min_argc) {/** last arg should be block quantity*/
			if (0 >= sscanf(argv[argc - 1], "%d", &blocks)){
				print_usage();
				return -EINVAL;
			}
			path = argv[argc - 2];
		} else {/** last arg should be diskname*/
			path = argv[argc - 1];
		}

		return mkfs_do_operation(blocks, path, fs_name, fs_type, operation_flag, fs_specific);
	}
	return 0;
}

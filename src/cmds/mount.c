/**
 * @file
 * @brief mount a filesystem
 *
 * @date 29.09.10
 * @author Nikolay Korotky
 */

#include <embox/cmd.h>
#include <getopt.h>
#include <fs/rootfs.h>
#include <fs/ramfs.h>
#include <fs/vfs.h>
#include <lib/list.h>
#include <cpio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <cmd/mount.h>

EMBOX_CMD(exec);

static void print_usage(void) {
	printf("Usage: mount [-h] src dir\n");
}

static int mount_dev(char *dev, char *dir) {
	mount_params_t param;
	node_t *dev_node;
	fs_drv_t * drv;

	param.dev = dev;
	param.dir = dir;

	if(NULL == (dev_node = vfs_find_node((const char *) dev, NULL))) {
		LOG_ERROR("mount: no such device\n");
		return -ENODEV;
	}
	param.dev_node = dev_node;

	drv = dev_node->fs_type;
	if (NULL == drv->fsop->mount) {
		return  -ENODEV;
	}
	return drv->fsop->mount((void *) &param);
}

static int exec(int argc, char **argv) {
	int opt;
	char *dev, *dir;

	getopt_init();
	while (-1 != (opt = getopt(argc, argv, "h"))) {
		switch (opt) {
		case '?':
			break;
		case 'h':
			print_usage();
			/* FALLTHROUGH */
		default:
			return 0;
		}
	}

	if (argc > 2) {
		dev = argv[argc - 2];
		dir = argv[argc - 1];
		return mount_dev(dev, dir);
	}
	return 0;
}

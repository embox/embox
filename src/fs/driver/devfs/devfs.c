/**
 * @file
 * @brief Device file system
 *
 * @date 14.10.10
 * @author Nikolay Korotky
 */

#include <fs/fs_drv.h>
#include <fs/vfs.h>

#include <embox/device.h>
#include <embox/block_dev.h>

static int devfs_init(void *par) {
	return 0;
}

static int devfs_mount(void *dev, void *dir) {
	int ret;
	struct node *node;
	mode_t mode;

	mode = S_IFDIR | S_IRALL | S_IWALL;

	assert(!vfs_lookup(NULL, dev)); // XXX remove it -- Eldar

	node = vfs_create(NULL, dev, mode);
	if (node) {
		return -1;
	}

	node->type = NODE_TYPE_DIRECTORY;

	ret = char_dev_init_all();
	if (ret != 0) {
		return ret;
	}

	return block_devs_init();
}

static fsop_desc_t devfs_fsop = { devfs_init, NULL, devfs_mount,
		NULL, NULL};

static const fs_drv_t devfs_drv = {
	.name = "devfs",
	.file_op = NULL,
	.fsop = &devfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(devfs_drv);

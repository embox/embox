/**
 * @file
 * @brief Device file system
 *
 * @date 14.10.10
 * @author Nikolay Korotky
 */

#include <fs/fs_driver.h>
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

	mode = S_IFDIR | S_IRALL | S_IWALL | S_IXALL;

	assert(!vfs_lookup(NULL, dev)); // XXX remove it -- Eldar

	node = vfs_create(NULL, dev, mode);
	if (!node) {
		return -1;
	}

	ret = char_dev_init_all();
	if (ret != 0) {
		return ret;
	}

	ret = flash_devs_init();
	if (ret != 0) {
		return ret;
	}

	return block_devs_init();
}

static struct fsop_desc devfs_fsop = {
	.init = devfs_init,
	.mount = devfs_mount,
};

static const struct fs_driver devfs_driver = {
	.name = "devfs",
	.file_op = NULL,
	.fsop = &devfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(devfs_driver);

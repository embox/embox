/**
 * @file
 * @brief Device file system
 *
 * @date 14.10.10
 * @author Nikolay Korotky
 */
#include <errno.h>
#include <sys/stat.h>

#include <fs/file_operation.h>
#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/inode.h>

#include <fs/file_desc.h>
#include <drivers/char_dev.h>
#include <drivers/block_dev.h>

extern int devfs_add_block(struct block_dev *dev);

static int devfs_mount(void *dev, void *dir) {
	int ret;
	struct path node, root;
	mode_t mode;
	struct block_dev **bdevs;
	int i;

	mode = S_IFDIR | S_IRALL | S_IWALL | S_IXALL;

	vfs_get_root_path(&root);

	if (0 != vfs_create(&root, dev, mode, &node)) {
		return -1;
	}

	bdevs = get_bdev_tab();
	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevs[i]) {
			devfs_add_block(bdevs[i]);
		}
	}

	ret = char_dev_init_all();
	if (ret != 0) {
		return ret;
	}

	ret = block_devs_init();
	if (ret != 0) {
		return ret;
	}

	return 0;
}

static struct fsop_desc devfs_fsop = {
	.mount = devfs_mount,
};

struct idesc *devfs_open(struct inode *node, struct idesc *idesc) {
	return idesc;
}

static struct file_operations devfs_fops = {
	.open = devfs_open
};

static const struct fs_driver devfs_driver = {
	.name = "devfs",
	.file_op = &devfs_fops,
	.fsop = &devfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(devfs_driver);

int devfs_add_block(struct block_dev *bdev) {
	struct path node, root;
	struct nas *nas;
	char full_path[PATH_MAX];

	vfs_get_root_path(&root);

	strcpy(full_path, "/dev/");
	strncat(full_path, bdev->name, PATH_MAX - strlen("/dev") - 1);

	if (0 != vfs_create(&root, full_path, S_IFBLK | S_IRALL | S_IWALL, &node)) {
		return -ENOENT;
	}

	bdev->dev_vfs_info = node.node;

	nas = node.node->nas;
	nas->fi->privdata = bdev;

	return 0;
}

int devfs_del_block(struct block_dev *bdev) {
	vfs_del_leaf(bdev->dev_vfs_info);

	return 0;
}

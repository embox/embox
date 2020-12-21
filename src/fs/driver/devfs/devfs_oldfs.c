/**
 * @file
 * @brief Device file system
 *
 * @date 14.10.10
 * @author Nikolay Korotky
 */
#include <errno.h>
#include <sys/stat.h>
#include <string.h>

#include <fs/file_operation.h>
#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/dir_context.h>

#include <fs/file_desc.h>
#include <drivers/block_dev.h>
#include <drivers/char_dev.h>
#include <drivers/device.h>

extern struct inode_operations devfs_iops;

static struct super_block *devfs_sb;

static int devfs_mount(struct super_block *sb, struct inode *dest) {
	int ret;

	dest->i_ops = &devfs_iops;

	ret = char_dev_init_all();
	if (ret != 0) {
		return ret;
	}

	ret = block_devs_init();
	if (ret != 0) {
		return ret;
	}

	devfs_sb = sb;

	return 0;
}

static struct fsop_desc devfs_fsop = {
	.mount = devfs_mount,
};

extern struct file_operations devfs_fops;
static const struct fs_driver devfs_driver = {
	.name = "devfs",
	.file_op = &devfs_fops,
	.fsop = &devfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(devfs_driver);

static int devfs_add_dev(struct dev_module *cdev, int flag) {
	struct path node;

	if (vfs_lookup("/dev", &node)) {
		return -ENOENT;
	}

	if (node.node == NULL) {
		return -ENODEV;
	}

	vfs_create_child(&node, cdev->name, flag | S_IRALL | S_IWALL, &node);
	if (!(node.node)) {
		return -1;
	}

	inode_priv_set(node.node, (void *) cdev);

	return 0;
}

static void devfs_del_node(const char *name) {
	struct path node;
	char full_path[PATH_MAX];

	strcpy(full_path, "/dev/");
	strncat(full_path, name, PATH_MAX - strlen("/dev") - 1);

	vfs_lookup(full_path, &node);

	vfs_del_leaf(node.node);
}

void devfs_notify_new_module(struct dev_module *devmod) {
	struct block_dev **bdevs;
	struct block_dev *bdev = dev_module_to_bdev(devmod);
	int max_id = block_dev_max_id();

	bdevs = get_bdev_tab();

	if (bdev != NULL) {
		for (int i = 0; i < max_id; i++) {
			if (bdevs[i] == bdev) {
				devfs_add_dev(devmod, S_IFBLK);
				return;
			}
		}
	}

	devfs_add_dev(devmod, S_IFCHR);
}

void devfs_notify_del_module(struct dev_module *devmod) {
	devfs_del_node(devmod->name);
}

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

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/dir_context.h>
#include <fs/super_block.h>
#include <fs/file_desc.h>
#include <fs/perm.h>

#include <drivers/block_dev.h>
#include <drivers/char_dev.h>
#include <drivers/device.h>

extern struct inode_operations devfs_iops;
extern struct file_operations devfs_fops;
extern int devfs_fill_sb(struct super_block *sb, const char *source);
extern int devfs_destroy_inode(struct inode *inode);

struct super_block_operations devfs_sbops = {
	//.open_idesc = devfs_open_idesc,
	.destroy_inode = devfs_destroy_inode,
};
#if 0
static int devfs_mount(struct super_block *sb, struct inode *dest) {
	return 0;
}
#endif
#if 0
static struct fsop_desc devfs_fsop = {
	//.mount = devfs_mount,
};
#endif

static const struct fs_driver devfs_driver = {
	.name = "devfs",
	.fill_sb   = devfs_fill_sb,

	//.fsop = &devfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(devfs_driver);

FILE_SYSTEM_AUTOMOUNT("/dev", devfs_driver);

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

struct block_dev *bdev_by_path(const char *source) {
	struct path dev_node;
	const char *lastpath;
	struct dev_module *devmod;

	if (source == NULL) {
		return NULL;
	}

	if (ENOERR != fs_perm_lookup(source, &lastpath, &dev_node)) {
		return NULL;
	}

	if (ENOERR != fs_perm_check(dev_node.node, S_IROTH | S_IXOTH)) {
		return NULL;
	}

	/* TODO: check if it's actually part of devfs? */
	devmod = inode_priv(dev_node.node);

	return dev_module_to_bdev(devmod);
}


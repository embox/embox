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
#include <drivers/char_dev.h>
#include <drivers/block_dev.h>
#include <drivers/device.h>

extern int devfs_add_block(struct block_dev *dev);
extern struct inode_operations devfs_iops;

static struct filesystem *devfs_file_system;

static int devfs_mount(void *dev, void *dir) {
	int ret;
	struct inode *dir_node;
	struct nas *dir_nas;

	dir_node = dir;
	dir_node->i_ops = &devfs_iops;
	dir_nas = dir_node->nas;

	ret = char_dev_init_all();
	if (ret != 0) {
		return ret;
	}

	ret = block_devs_init();
	if (ret != 0) {
		return ret;
	}
	devfs_file_system = filesystem_create("devfs");
	if (devfs_file_system == NULL) {
		return -ENOMEM;
	}
	dir_nas->fs = devfs_file_system;

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

static int devfs_add_char(struct dev_module *cdev, struct inode **inode) {
	struct path node;
	struct nas *dev_nas;

	if (vfs_lookup("/dev", &node)) {
		return -ENOENT;
	}

	if (node.node == NULL) {
		return -ENODEV;
	}

	vfs_create_child(&node, cdev->name, S_IFCHR | S_IRALL | S_IWALL, &node);
	if (!(node.node)) {
		return -1;
	}

	dev_nas = node.node->nas;
	dev_nas->fs = devfs_file_system;
	node.node->nas->fi->privdata = (void *)cdev;

	*inode = node.node;

	return 0;
}

void devfs_fill_inode(struct inode *inode, struct dev_module *devmod, int flags) {
	assert(inode);
	assert(devmod);

	inode->nas->fi->privdata = devmod;
	inode->nas->fs = devfs_file_system;
	inode->i_mode = flags;
}

extern struct dev_module **get_cdev_tab();
/**
 * @brief Iterate elements of /dev
 *
 * @note Devices are iterated type by type
 * @note Two least significant bits of fs-specific pointer is dev type, the
 * rest is dev number in dev tab
 *
 * @param next Inode to be filled
 * @param parent Ignored
 * @param ctx
 *
 * @return Negative error code
 */
static int devfs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx) {
	int i;
	struct block_dev **bdevtab = get_bdev_tab();
	struct dev_module **cdevtab = get_cdev_tab();

	i = ((intptr_t) ctx->fs_ctx);

	for (; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevtab[i]) {
			ctx->fs_ctx = (void *)(intptr_t)i + 1;
			devfs_fill_inode(next, bdevtab[i]->dev_module, S_IFBLK | S_IRALL | S_IWALL);
			strncpy(next->name, bdevtab[i]->name, sizeof(next->name) - 1);
			next->name[sizeof(next->name) - 1] = '\0';
			return 0;
		}
	}

	/* Dynamically allocated devices */
	for (; i < (MAX_CDEV_QUANTITY + MAX_BDEV_QUANTITY); i++) {
		if (cdevtab[i - MAX_BDEV_QUANTITY]) {
			ctx->fs_ctx = (void *) ((intptr_t) i + 1);
			devfs_fill_inode(next, cdevtab[i - MAX_BDEV_QUANTITY], S_IFCHR | S_IRALL | S_IWALL);
			strncpy(next->name, cdevtab[i - MAX_BDEV_QUANTITY]->name, sizeof(next->name) - 1);
			next->name[sizeof(next->name) - 1] = '\0';
			return 0;
		}
	}

	/* End of directory */
	return -1;
}

/**
 * @brief Find file in directory
 *
 * @param name Name of file
 * @param dir  Pointer to directory
 *
 * @return Pointer to inode structure or NULL if failed
 */
static struct inode *devfs_lookup(char const *name, struct dentry const *dir) {
	struct block_dev **bdevs;
	int i;
	struct dev_module **cdevtab;

	cdevtab = get_cdev_tab();
	bdevs = get_bdev_tab();
	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (0 == strncmp(bdevs[i]->name, name, sizeof(bdevs[i]->name))) {
			if (devfs_add_block(bdevs[i])) {
				return NULL;
			}
			return bdevs[i]->dev_vfs_info;
		}
	}

	/* Dynamically allocated devices */
	for (i = 0; i < MAX_CDEV_QUANTITY; i++) {
		if (0 == strncmp(cdevtab[i]->name, name, sizeof(cdevtab[i]->name))) {
			struct inode *inode;
			if (devfs_add_char(cdevtab[i], &inode)) {
				return NULL;
			}
			return inode;
		}
	}
	return NULL;
}

static int devfs_create(struct inode *i_new, struct inode *i_dir, int mode) {
	return 0;
}

struct inode_operations devfs_iops = {
	.lookup   = devfs_lookup,
	.iterate  = devfs_iterate,
	.create   = devfs_create,
};

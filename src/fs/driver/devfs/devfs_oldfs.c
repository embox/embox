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

/* Common part */
struct idesc *devfs_open(struct inode *node, struct idesc *desc);
int devfs_create(struct inode *i_new, struct inode *i_dir, int mode);

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

static int devfs_add_block(struct dev_module *devmod) {
	struct block_dev *bdev = devmod->dev_priv;
	struct path node, root;
	char full_path[PATH_MAX];

	vfs_get_root_path(&root);

	strcpy(full_path, "/dev/");
	strncat(full_path, bdev->name, PATH_MAX - strlen("/dev") - 1);

	if (0 != vfs_create(&root, full_path, S_IFBLK | S_IRALL | S_IWALL, &node)) {
		return -ENOENT;
	}

	bdev->dev_vfs_info = node.node;

	inode_priv_set(node.node, devmod);

	return 0;
}

static int devfs_add_char(struct dev_module *cdev, struct inode **inode) {
	struct path node;

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

	inode_priv_set(node.node, (void *) cdev);
	if (inode) {
		*inode = node.node;
	}

	return 0;
}

void devfs_fill_inode(struct inode *inode, struct dev_module *devmod, int flags) {
	assert(inode);
	assert(devmod);

	inode_priv_set(inode, devmod);
	inode->i_mode = flags;
}

extern int devfs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx);
extern struct dev_module **get_cdev_tab(void);

/**
 * @brief Find file in directory
 *
 * @param name Name of file
 * @param dir  Not used in this driver as we assume there are no nested
 * directories
 *
 * @return Pointer to inode structure or NULL if failed
 */
static struct inode *devfs_lookup(char const *name, struct inode const *dir) {
	struct block_dev **bdevs;
	int i;
	struct dev_module **cdevtab;

	cdevtab = get_cdev_tab();
	bdevs = get_bdev_tab();
	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevs[i] == NULL) {
			continue;
		}

		if (0 == strncmp(bdevs[i]->name, name, sizeof(bdevs[i]->name))) {
			if (devfs_add_block(bdevs[i]->dev_module)) {
				return NULL;
			}
			return bdevs[i]->dev_vfs_info;
		}
	}

	/* Dynamically allocated devices */
	for (i = 0; i < MAX_CDEV_QUANTITY; i++) {
		if (cdevtab[i] == NULL) {
			continue;
		}

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

struct inode_operations devfs_iops = {
	.lookup   = devfs_lookup,
	.iterate  = devfs_iterate,
	.create   = devfs_create,
};

void devfs_notify_new_module(struct dev_module *devmod) {
	struct block_dev **bdevs;
	void *priv = devmod->dev_priv;
	int max_id = block_dev_max_id();

	bdevs = get_bdev_tab();

	if (priv != NULL) {
		for (int i = 0; i < max_id; i++) {
			if (bdevs[i] == priv) {
				devfs_add_block(devmod);
				return;
			}
		}
	}

	devfs_add_char(devmod, NULL);
}

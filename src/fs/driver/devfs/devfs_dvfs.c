/**
 * @file devfs_dvfs.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-09-28
 */

/* IMPORTANT NOTE
 *
 * In current implementation, devfs is supposed to have no nested folders,
 * so all operations are threated just like you use /dev directory,
 * or whereever did you mount devfs. Future release should fix it, probably
 */
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#include <util/err.h>

#include <drivers/char_dev.h>
#include <drivers/device.h>
#include <framework/mod/options.h>
#include <fs/dvfs.h>
#include <kernel/printk.h>
#include <util/array.h>

#include <module/embox/driver/block_common.h>
#include <module/embox/driver/char_dev_dvfs.h>

#define MAX_BDEV_QUANTITY OPTION_MODULE_GET(embox__driver__block_common, NUMBER, dev_quantity)
#define MAX_CDEV_QUANTITY OPTION_MODULE_GET(embox__driver__char_dev_dvfs, NUMBER, dev_quantity)

/**
 * @brief Do nothing
 *
 * @param inode
 *
 * @return
 */
static int devfs_destroy_inode(struct inode *inode) {
	return 0;
}

void devfs_fill_inode(struct inode *inode,
		struct dev_module *devmod, int flags) {
	assert(inode);
	assert(devmod);

	inode->i_data = devmod;
	inode->flags |= flags;
}

ARRAY_SPREAD_DECLARE(const struct dev_module, __char_device_registry);
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
static int devfs_iterate(struct inode *next, struct inode *parent, struct dir_ctx *ctx) {
	int i, j;
	struct dev_module *dev_module;
	struct block_dev **bdevtab = get_bdev_tab();
	struct dev_module **cdevtab = get_cdev_tab();
	switch ((intptr_t)ctx->fs_ctx & 3) {
	case 0:
		/* Block device */
		for (i = ((intptr_t) ctx->fs_ctx >> 2); i < MAX_BDEV_QUANTITY; i++) {
			if (bdevtab[i]) {
				ctx->fs_ctx = (void*) ((intptr_t) ctx->fs_ctx + 0x4);
				devfs_fill_inode(next, bdevtab[i]->dev_module, S_IFBLK);
				next->length = bdevtab[i]->size;
				return 0;
			}
		}
		/* Fall through */
		ctx->fs_ctx = (void*) ((int) 0x1);
	case 1:
		/* Char device */
		i = 0;
		/* Statically registered devices */
		array_spread_foreach_ptr(dev_module, __char_device_registry) {
			if (i++ == (intptr_t) ctx->fs_ctx >> 2) {
				ctx->fs_ctx = (void*) ((intptr_t) ctx->fs_ctx + 0x4);
				devfs_fill_inode(next, dev_module, S_IFCHR);
				return 0;
			}
		}

		/* Dynamically allocated devices */
		for (j = 0; j < MAX_CDEV_QUANTITY; j++) {
			if (cdevtab[j] && i++ == (intptr_t) ctx->fs_ctx >> 2) {
				ctx->fs_ctx = (void *) ((intptr_t) ctx->fs_ctx + 0x4);
				devfs_fill_inode(next, cdevtab[j], S_IFCHR);
				return 0;
			}
		}

		/* Fall through */
		ctx->fs_ctx = (void*) ((int) 0x1);
	case 2:
		/* Fall through */
	case 3:
	default:
		/* wtf */
		return -1;
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
	int i;
	struct inode *node;
	struct dev_module *dev_module;
	struct block_dev **bdevtab = get_bdev_tab();
	struct dev_module **cdevtab = get_cdev_tab();

	if (NULL == (node = dvfs_alloc_inode(dir->d_sb))) {
		return NULL;
	}

	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevtab[i] && !strcmp(bdevtab[i]->name, name)) {
			devfs_fill_inode(node, bdevtab[i]->dev_module, S_IFBLK);
			node->length = bdevtab[i]->size;
			return node;
		}
	}

	/* Statically registered char devices */
	array_spread_foreach_ptr(dev_module, __char_device_registry) {
		if (!strcmp(dev_module->name, name)) {
			devfs_fill_inode(node, dev_module, S_IFCHR);
			return node;
		}
	}

	/* Dynamically allocated char devices */
	for (i = 0; i < MAX_CDEV_QUANTITY; i++) {
		if (cdevtab[i] && !strcmp(cdevtab[i]->name, name)) {
			devfs_fill_inode(node, cdevtab[i], S_IFCHR);
			return node;
		}
	}

	dvfs_destroy_inode(node);

	return NULL;
}

static int devfs_mount_end(struct super_block *sb) {
	return block_devs_init();
}

static struct idesc *devfs_open(struct inode *node, struct idesc *desc) {
	struct dev_module *dev;

	assert(node->i_data);

	if (S_ISBLK(node->flags)) {
		return desc;
	}

	dev = node->i_data;
	assert(dev->dev_open);

	return dev->dev_open(dev, dev->dev_priv);
}


static int devfs_pathname(struct inode *node, char *buf, int flags) {

	if ((node->flags & S_IFBLK) == S_IFBLK ||
		(node->flags & S_IFCHR) == S_IFCHR) {
		struct dev_module *dev_module = node->i_data;
		strncpy(buf, dev_module->name, DENTRY_NAME_LEN);
	} else {
		return -1; /* Wrong flags */
	}

	return 0;
}

static int devfs_create(struct inode *i_new, struct inode *i_dir, int mode) {
	return 0;
}

static int devfs_ioctl(struct file *desc, int request, void *data) {
	return 0;
}

extern struct idesc_ops idesc_bdev_ops;
/* Call device-specific open() handler */
static struct idesc *devfs_open_idesc(struct lookup *l, int __oflag) {
	struct inode  *i_no;
	struct dev_module *dev;
	struct idesc *desc;

	assert(l);
	assert(l->item);
	assert(l->item->d_inode);

	i_no = l->item->d_inode;
	dev = i_no->i_data;

	if (S_ISBLK(i_no->flags)) {
		/* XXX */
		desc = dvfs_file_open_idesc(l, __oflag);

		desc->idesc_ops = &idesc_bdev_ops;

		return desc;
	}

	assert(dev);
	if(__oflag & O_PATH) {
		return char_dev_idesc_create(NULL);
	}
	assert(dev->dev_open);
	desc = dev->dev_open(dev, (void *)(uintptr_t) __oflag);

	return desc;
}

struct super_block_operations devfs_sbops = {
	.open_idesc = devfs_open_idesc,
	.destroy_inode = devfs_destroy_inode,
};

struct inode_operations devfs_iops = {
	.lookup   = devfs_lookup,
	.iterate  = devfs_iterate,
	.pathname = devfs_pathname,
	.create   = devfs_create,
};

struct file_operations devfs_fops = {
	.open  = devfs_open,
	.ioctl = devfs_ioctl,
};

static int devfs_fill_sb(struct super_block *sb, struct file *bdev_file) {
	sb->sb_iops = &devfs_iops;
	sb->sb_fops = &devfs_fops;
	sb->sb_ops  = &devfs_sbops;
	if (bdev_file) {
		return -1;
	}
	return 0;
}

static const struct dumb_fs_driver devfs_dumb_driver = {
	.name      = "devfs",
	.fill_sb   = devfs_fill_sb,
	.mount_end = devfs_mount_end,
};

ARRAY_SPREAD_DECLARE(const struct dumb_fs_driver *const, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &devfs_dumb_driver);

static struct auto_mount devfs_auto_mount = {
	.mount_path = "/dev",
	.fs_driver  = (struct dumb_fs_driver *)&devfs_dumb_driver,
};

ARRAY_SPREAD_DECLARE(const struct auto_mount *const, auto_mount_tab);
ARRAY_SPREAD_ADD(auto_mount_tab, &devfs_auto_mount);

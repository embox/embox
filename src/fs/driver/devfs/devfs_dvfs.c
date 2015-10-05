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

#include <string.h>
#include <sys/stat.h>

#include <drivers/char_dev.h>
#include <framework/mod/options.h>
#include <fs/dvfs.h>
#include <kernel/printk.h>
#include <util/array.h>

#include <module/embox/driver/block_common.h>

#define MAX_BDEV_QUANTITY OPTION_MODULE_GET(embox__driver__block_common, NUMBER, dev_quantity)

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

void devfs_fill_inode(struct inode *inode, void *dev, int flags) {
	inode->i_data = dev;
	inode->flags |= flags;
}

ARRAY_SPREAD_DECLARE(const struct device_module, __char_device_registry);
extern struct block_dev **get_bdev_tab();
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
	int i;
	struct device_module *dev_module;
	struct block_dev **bdevtab = get_bdev_tab();
	switch ((int)ctx->fs_ctx & 3) {
	case 0:
		/* Block device */
		for (i = ((int) ctx->fs_ctx >> 2); i < MAX_BDEV_QUANTITY; i++)
			if (bdevtab[i]) {
				ctx->fs_ctx = (void*) ((int) ctx->fs_ctx + 0x4);
				devfs_fill_inode(next, bdevtab[i], S_IFBLK);
				return 0;
			}
		/* Fall through */
		ctx->fs_ctx = (void*) ((int) 0x1);
	case 1:
		/* Char device */
		i = 0;
		array_spread_foreach_ptr(dev_module, __char_device_registry) {
			if (i++ == (int) ctx->fs_ctx >> 2) {
				ctx->fs_ctx = (void*) ((int) ctx->fs_ctx + 0x4);
				devfs_fill_inode(next, dev_module, S_IFCHR);
				return 0;
			}
		}
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

static struct inode *devfs_lookup(char const *name, struct dentry const *dir) {
	return NULL;
}

static int devfs_mount_end(struct super_block *sb) {
	return 0;
}

static int devfs_open(struct inode *node, struct file *file) {
	return 0;
}

enum device_type { NOT_DEVICE, BLOCK, CHAR, FLASH };

static size_t devfs_read(struct file *desc, void *buf, size_t size) {
#if 0
	void *dev = dev_by_desc(desc);
	enum device_type type = dev_type(dev);

	switch (type) {
	case BLOCK:
		block_dev_read( ... );
		break;
	default:
		printk("Unknown device type!\n");
	}
#endif
	return 0;
}

static int devfs_pathname(struct inode *node, char *buf, int flags) {
	struct device_module *dev_module;
	struct block_dev *bdev;

	if ((node->flags & S_IFBLK) == S_IFBLK) {
		bdev = node->i_data;
		strncpy(buf, bdev->name, DENTRY_NAME_LEN);
	} else if ((node->flags & S_IFCHR) == S_IFCHR) {
		dev_module = node->i_data;
		strncpy(buf, dev_module->name, DENTRY_NAME_LEN);
	} else {
		return -1; /* Wrong flags */
	}

	return 0;
}

static int devfs_create(struct inode *i_new, struct inode *i_dir, int mode) {
	return 0;
}

static int devfs_ioctl(struct file *desc, int request, ...) {
	return 0;
}

struct super_block_operations devfs_sbops = {
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
	.read  = devfs_read,
	.ioctl = devfs_ioctl,
};

static int devfs_fill_sb(struct super_block *sb, struct block_dev *dev) {
	sb->sb_iops = &devfs_iops;
	sb->sb_fops = &devfs_fops;
	sb->sb_ops  = &devfs_sbops;
	return 0;
}

static struct dumb_fs_driver devfs_dumb_driver = {
	.name      = "devfs",
	.fill_sb   = devfs_fill_sb,
	.mount_end = devfs_mount_end,
};

ARRAY_SPREAD_DECLARE(struct dumb_fs_driver *, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &devfs_dumb_driver);

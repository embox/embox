/**
 * @file devfs_common.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 30.01.2020
 */
#include <assert.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>

#include <drivers/block_dev.h>
#include <drivers/char_dev.h>
#include <drivers/device.h>
#include <fs/dir_context.h>
#include <fs/file_desc.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/super_block.h>
#include <kernel/task/resource/idesc.h>
#include <lib/libds/array.h>

extern struct block_dev **get_bdev_tab(void);

static struct idesc *devfs_open(struct inode *node, struct idesc *idesc,
    int __oflag) {
	extern struct idesc_ops idesc_bdev_ops;

	struct char_dev *cdev;

	if (S_ISBLK(node->i_mode)) {
		idesc_init(idesc, &idesc_bdev_ops, __oflag);
		return idesc;
	}

	cdev = inode_priv(node);
	assert(cdev);

	return char_dev_open(cdev, __oflag);
}

static int devfs_ioctl(struct file_desc *desc, int request, void *data) {
	return 0;
}

static int devfs_create(struct inode *i_new, struct inode *i_dir, int mode) {
	return 0;
}

struct file_operations devfs_fops = {
    .open = devfs_open,
    .ioctl = devfs_ioctl,
};

static void devfs_fill_inode(struct inode *inode, void *priv, int flags) {
	assert(inode);
	assert(priv);

	inode_priv_set(inode, priv);
	inode->i_mode = flags;
}

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
	int i;
	struct inode *node;
	struct char_dev *cdev;
	struct block_dev **bdevtab = get_bdev_tab();

	if (NULL == (node = inode_new(dir->i_sb))) {
		return NULL;
	}

	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevtab[i] && !strcmp(block_dev_name(bdevtab[i]), name)) {
			devfs_fill_inode(node, block_dev_to_device(bdevtab[i]), S_IFBLK);
			node->i_size = bdevtab[i]->size;
			return node;
		}
	}

	if ((cdev = char_dev_find(name))) {
		devfs_fill_inode(node, cdev, S_IFCHR);
		return node;
	}

	inode_del(node);

	return NULL;
}

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
static int devfs_iterate(struct inode *next, char *name, struct inode *parent,
    struct dir_ctx *ctx) {
	struct block_dev **bdevtab = get_bdev_tab();
	struct char_dev *cdev;
	uintptr_t i;

	/* All block devices */
	for (i = ((uintptr_t)ctx->fs_ctx); i < MAX_BDEV_QUANTITY; i++) {
		if (bdevtab[i]) {
			ctx->fs_ctx = (void *)(intptr_t)i + 1;
			devfs_fill_inode(next, block_dev_to_device(bdevtab[i]),
			    S_IFBLK | S_IRALL | S_IWALL);
			strncpy(name, block_dev_name(bdevtab[i]), NAME_MAX - 1);
			name[NAME_MAX - 1] = '\0';
			return 0;
		}
	}

	/* All char device */
	cdev = (i == MAX_BDEV_QUANTITY) ? NULL : ctx->fs_ctx;
	if ((cdev = char_dev_iterate(cdev))) {
		ctx->fs_ctx = (void *)cdev;
		devfs_fill_inode(next, cdev, S_IFCHR | S_IRALL | S_IWALL);
		strcpy(name, cdev->name);
		return 0;
	}

	/* End of directory */
	return -1;
}

/**
 * @brief Do nothing
 *
 * @param inode
 *
 * @return
 */
int devfs_destroy_inode(struct inode *inode) {
	return 0;
}

struct inode_operations devfs_iops = {
    .ino_lookup = devfs_lookup,
    .ino_iterate = devfs_iterate,
    .ino_create = devfs_create,
};

extern struct super_block_operations devfs_sbops;
int devfs_fill_sb(struct super_block *sb, const char *source) {
	if (source) {
		return -1;
	}

	sb->sb_iops = &devfs_iops;
	sb->sb_fops = &devfs_fops;
	sb->sb_ops = &devfs_sbops;

	return block_devs_init();
}

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
#include <drivers/block_dev.h>
#include <drivers/device.h>
#include <framework/mod/options.h>
#include <fs/dvfs.h>

#include <util/array.h>

#include <module/embox/driver/block_dev.h>

/* Common part */
int devfs_create(struct inode *i_new, struct inode *i_dir, int mode);

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
	inode->i_mode = flags;
}

extern int devfs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx);
ARRAY_SPREAD_DECLARE(const struct dev_module, __char_device_registry);
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
	int i;
	struct inode *node;
	struct block_dev **bdevtab = get_bdev_tab();
	struct dev_module **cdevtab = get_cdev_tab();

	if (NULL == (node = dvfs_alloc_inode(dir->i_sb))) {
		return NULL;
	}

	for (i = 0; i < MAX_BDEV_QUANTITY; i++) {
		if (bdevtab[i] && !strcmp(bdevtab[i]->name, name)) {
			devfs_fill_inode(node, bdevtab[i]->dev_module, S_IFBLK);
			node->length = bdevtab[i]->size;
			return node;
		}
	}

	for (i = 0; i < MAX_CDEV_QUANTITY; i++) {
		if (cdevtab[i] && !strcmp(cdevtab[i]->name, name)) {
			devfs_fill_inode(node, cdevtab[i], S_IFCHR);
			return node;
		}
	}

	dvfs_destroy_inode(node);

	return NULL;
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

	if (S_ISBLK(i_no->i_mode)) {
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
	.create   = devfs_create,
};

extern struct file_operations devfs_fops ;
static int devfs_fill_sb(struct super_block *sb, const char *source) {
	if (source) {
		return -1;
	}

	sb->sb_iops = &devfs_iops;
	sb->sb_fops = &devfs_fops;
	sb->sb_ops  = &devfs_sbops;

	char_dev_init_all();
	return block_devs_init();
}

static const struct fs_driver devfs_dumb_driver = {
	.name      = "devfs",
	.fill_sb   = devfs_fill_sb,
};

ARRAY_SPREAD_DECLARE(const struct fs_driver *const, fs_drivers_registry);
ARRAY_SPREAD_ADD(fs_drivers_registry, &devfs_dumb_driver);

static struct auto_mount devfs_auto_mount = {
	.mount_path = "/dev",
	.fs_driver  = (struct fs_driver *)&devfs_dumb_driver,
};

ARRAY_SPREAD_DECLARE(const struct auto_mount *const, auto_mount_tab);
ARRAY_SPREAD_ADD(auto_mount_tab, &devfs_auto_mount);

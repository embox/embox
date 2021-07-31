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

extern struct idesc_ops idesc_bdev_ops;
extern int devfs_destroy_inode(struct inode *inode);

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

extern int devfs_fill_sb(struct super_block *sb, const char *source);

static const struct fs_driver devfs_dumb_driver = {
	.name      = "devfs",
	.fill_sb   = devfs_fill_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(devfs_dumb_driver);

static struct auto_mount devfs_auto_mount = {
	.mount_path = "/dev",
	.fs_driver  = (struct fs_driver *)&devfs_dumb_driver,
};

ARRAY_SPREAD_DECLARE(const struct auto_mount *const, auto_mount_tab);
ARRAY_SPREAD_ADD(auto_mount_tab, &devfs_auto_mount);

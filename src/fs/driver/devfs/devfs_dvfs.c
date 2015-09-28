/**
 * @file devfs_dvfs.c
 * @brief
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-09-28
 */

#include <fs/dvfs.h>
#include <util/array.h>

static int devfs_destroy_inode(struct inode *inode) {
	return 0;
}

static int devfs_iterate(struct inode *next, struct inode *parent, struct dir_ctx *ctx) {
	return 0;
}

static struct inode *devfs_lookup(char const *name, struct dentry const *dir) {
	return NULL;
}

static int devfs_pathname(struct inode *inode, char *buf, int flags) {
	return 0;
}

static int devfs_mount_end(struct super_block *sb) {
	return 0;
}

static int devfs_open(struct inode *node, struct file *file) {
	return 0;
}

static size_t devfs_read(struct file *desc, void *buf, size_t size) {
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

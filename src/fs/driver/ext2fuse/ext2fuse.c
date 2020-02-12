/**
 * @file
 * @brief ext2fuse support
 *
 * @date 16.11.2015
 * @author Alexander Kalmuk
 */

#include <stddef.h>

#include <fs/dvfs.h>

#include <fs/fuse_driver.h>

struct fuse_sb_priv_data ext2fuse_sb_priv_data;

static int ext2fuse_fill_sb(struct super_block *sb, const char *source) {
	assert(sb);

	sb->sb_data = &ext2fuse_sb_priv_data;

	sb->sb_iops = (struct inode_operations *)&fuse_iops;
	sb->sb_fops = (struct file_operations *)&fuse_fops;
	sb->sb_ops  = (struct super_block_operations *)&fuse_sbops;
	sb->bdev = NULL;

	return 0;
}

static const struct fs_driver ext2fuse_dumb_driver = {
	.name      = "ext2fuse",
	.fill_sb   = ext2fuse_fill_sb,
};

ARRAY_SPREAD_DECLARE(const struct fs_driver *const, fs_drivers_registry);
ARRAY_SPREAD_ADD(fs_drivers_registry, &ext2fuse_dumb_driver);

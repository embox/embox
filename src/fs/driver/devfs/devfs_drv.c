/**
 * @file
 * @brief Device file system
 *
 * @date 14.10.10
 * @author Nikolay Korotky
 */

#include <fs/fs_driver.h>

#include <fs/inode.h>

#include <fs/super_block.h>

extern struct super_block_operations devfs_sbops;

extern int devfs_fill_sb(struct super_block *sb, const char *source);

static const struct fs_driver devfs_driver = {
	.name = "devfs",
	.fill_sb   = devfs_fill_sb,

};

DECLARE_FILE_SYSTEM_DRIVER(devfs_driver);

FILE_SYSTEM_AUTOMOUNT("/dev", devfs_driver);

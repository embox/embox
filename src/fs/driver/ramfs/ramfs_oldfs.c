/**
 * @file
 * @brief Tmp file system
 *
 * @date 12.11.12
 * @author Andrey Gazukin
 */

#include <errno.h>

#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/super_block.h>

#include "ramfs.h"

extern int ramfs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx);

struct inode_operations ramfs_iops = {
	.ino_create = ramfs_create,
	.ino_remove = ramfs_delete,
	.ino_iterate  = ramfs_iterate,
	.ino_truncate = ramfs_truncate,
};

struct super_block_operations ramfs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = ramfs_destroy_inode,
};

#if 0
static int ramfs_mount(struct super_block *sb, struct inode *dest) {
	return 0;
}
#endif

static struct fsop_desc ramfs_fsop = {
	//.mount = ramfs_mount,
};

static struct fs_driver ramfs_driver = {
	.name    = "ramfs",
	.format = ramfs_format,
	.fill_sb = ramfs_fill_sb,

	.fsop    = &ramfs_fsop,
};

DECLARE_FILE_SYSTEM_DRIVER(ramfs_driver);

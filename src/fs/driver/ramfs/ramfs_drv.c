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
#include <fs/ramfs.h>

extern int ramfs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx);
extern struct inode *ramfs_ilookup(struct inode *node, char const *name, struct inode const *dir);
/* Declaration of operations */
struct inode_operations ramfs_iops = {
	.ino_create   = ramfs_create,
	.ino_lookup   = ramfs_ilookup,
	.ino_remove   = ramfs_delete,
	.ino_iterate  = ramfs_iterate,
	.ino_truncate = ramfs_truncate,
};

extern struct super_block_operations ramfs_sbops;

static struct fs_driver ramfs_driver = {
	.name    = "ramfs",
	.format = ramfs_format,
	.fill_sb = ramfs_fill_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(ramfs_driver);

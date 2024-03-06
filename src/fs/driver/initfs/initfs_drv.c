/**
 * @file
 * @details Read-only filesystem with direct address space mapping.
 *
 * @date 7 May 2015
 * @author Anton Bondarev
 *	        - initial implementation
 * @author Nikolay Korotky
 *	        - rework using vfs
 * @author Eldar Abusalimov
 *	        - rework mount to use cpio_parse_entry
 * @author Denis Deryugin
 *              - port from old VFS
 *
 * @note   Initfs is based on CPIO archive format. By design, this format
 *         has no directory abstraction, as all files are stored with full
 *         path names. Because of this it could be tricky to handle some
 *         VFS calls.
 */

#include <fs/inode.h>
#include <fs/inode_operation.h>
#include <fs/fs_driver.h>

extern struct inode *initfs_lookup(struct inode *node, char const *name, struct inode const *dir);

extern struct super_block_operations initfs_sbops;
extern int initfs_create(struct inode *i_new, struct inode *i_dir, int mode);
extern int initfs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx);

struct inode_operations initfs_iops = {
	.ino_create   = initfs_create,
	.ino_lookup   = initfs_lookup,
	.ino_iterate  = initfs_iterate,
};

extern int initfs_fill_sb(struct super_block *sb, const char *source);

static const struct fs_driver initfs_driver = {
	.name      = "initfs",
	.fill_sb   = initfs_fill_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(initfs_driver);

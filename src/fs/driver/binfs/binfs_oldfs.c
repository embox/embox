/**
 * @file
 * @brief
 *
 * @author  Anton Kozlov
 * @date    23.05.2014
 */

#include <fs/fs_driver.h>
#include <fs/super_block.h>
#include <fs/dir_context.h>

#define BINFS_NAME "binfs"

struct inode *binfs_lookup(struct inode *node, char const *name, struct inode const *dir) {
	return NULL;
}

extern int binfs_destroy_inode(struct inode *inode) ;

extern struct file_operations binfs_fops;

extern struct inode_operations binfs_iops;

extern int binfs_fill_sb(struct super_block *sb, const char *source);

struct super_block_operations binfs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = binfs_destroy_inode,
};

static struct fs_driver binfs_driver = {
	.name = BINFS_NAME,
	.fill_sb   = binfs_fill_sb,
};

DECLARE_FILE_SYSTEM_DRIVER(binfs_driver);

FILE_SYSTEM_AUTOMOUNT("/bin", binfs_driver);


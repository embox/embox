/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#include <fs/inode.h>
#include <fs/super_block.h>

extern int ext3fs_destroy_inode(struct inode *inode);

struct super_block_operations ext3fs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = &ext3fs_destroy_inode,
};

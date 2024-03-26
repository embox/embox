/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#include <fs/inode.h>
#include <fs/super_block.h>

extern int e2fs_destroy_inode(struct inode *inode);
extern struct idesc *dvfs_file_open_idesc(struct lookup *lookup, int __oflag);

struct super_block_operations e2fs_sbops = {
	.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = e2fs_destroy_inode,
};

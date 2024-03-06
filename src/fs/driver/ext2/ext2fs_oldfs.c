/**
 * @file
 * @brief ext file system
 *
 * @date 04.12.2012
 * @author Andrey Gazukin
 */

#include <stddef.h>

#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/inode_operation.h>

struct inode *ext2fs_lookup(struct inode *node, char const *name, struct inode const *dir) {
	return NULL;
}

extern int e2fs_destroy_inode(struct inode *inode);

struct super_block_operations e2fs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = e2fs_destroy_inode,
};

/**
 * @file
 * @brief
 *
 * @date 10.02.2024
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <string.h>

#include <fs/inode.h>
#include <fs/super_block.h>

#include <fs/iso9660.h>

extern int cdfs_destroy_inode(struct inode *inode);

struct inode *cdfs_ilookup(struct inode *node, char const *name, struct inode const *dir) {
	return NULL;
}

struct super_block_operations cdfs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = cdfs_destroy_inode,
};

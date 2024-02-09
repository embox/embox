/**
 * @file
 * @brief Tmp file system
 *
 * @date 12.11.12
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stddef.h>

#include <fs/inode.h>
#include <fs/super_block.h>

#include "ramfs.h"

struct inode *ramfs_ilookup(char const *name, struct inode const *dir) {
	return NULL;
}

struct super_block_operations ramfs_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = ramfs_destroy_inode,
};

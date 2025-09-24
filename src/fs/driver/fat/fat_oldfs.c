/**
 * @file
 * @brief Implementation of FAT driver
 *
 * @date 28.03.2012
 * @author Andrey Gazukin
 */

#include <stddef.h>

#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/fat.h>

struct inode *fat_ilookup(struct inode *node, char const *name, struct inode const *dir) {
	return NULL;
}

extern int fat_destroy_inode(struct inode *inode);
struct super_block_operations fat_sbops = {
	//.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = fat_destroy_inode,
};


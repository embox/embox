/**
 * @file
 * @brief Tmp file system
 *
 * @date 12.11.12
 * @author Andrey Gazukin
 */

#include <stddef.h>
#include <string.h>
#include <sys/stat.h>

#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/ramfs.h>

extern struct ramfs_file_info ramfs_files[RAMFS_FILES];

struct inode *ramfs_ilookup(struct inode *node, char const *name, struct inode const *dir) {
	struct super_block *sb;

	assert(dir);
	assert(dir->i_sb);
	assert(dir->i_sb->sb_data);

	sb = dir->i_sb;

	for (int i = 0; i < RAMFS_FILES; i++) {
		if (ramfs_files[i].fsi != sb->sb_data) {
			continue;
		}

		if (strcmp(name, ramfs_files[i].name)) {
			continue;
		}

		node->i_privdata = &ramfs_files[i];
		node->i_no = ramfs_files[i].index;
		node->i_size = ramfs_files[i].length;
		node->i_mode = ramfs_files[i].mode & (S_IFMT | S_IRWXA);

		return node;
	}

	return NULL;
}

extern struct idesc *dvfs_file_open_idesc(struct lookup *lookup, int __oflag);

struct super_block_operations ramfs_sbops = {
	.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = ramfs_destroy_inode,
};

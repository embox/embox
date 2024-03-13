/**
 * @file
 * @brief
 *
 * @date 10.02.2024
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/dentry.h>

#include <fs/iso9660.h>

extern struct idesc *dvfs_file_open_idesc(struct lookup *lookup, int __oflag);
extern int cdfs_destroy_inode(struct inode *inode);
extern int cdfs_find_in_dir(struct cdfs_fs_info *cdfs, int dir, char *name,
								int len, iso_directory_record_t **dirrec);
extern int cdfs_find_dir(struct cdfs_fs_info *cdfs, char *name, int len);
extern int cdfs_alloc_inode_priv(struct inode* node);
extern int cdfs_fill_node(struct inode* node, char *name, 
					struct cdfs_fs_info *cdfs, iso_directory_record_t *rec);

struct inode *cdfs_ilookup(struct inode *node, char const *name, struct inode const *dir) {
	struct super_block *sb;
	struct cdfs_fs_info *fsi;
	iso_directory_record_t *rec;
	int res;
	int n;
	char *dir_name;

	assert(name);
	assert(S_ISDIR(dir->i_mode));

	sb = dir->i_sb;
	fsi = sb->sb_data;

	if (dir == dir->i_sb->sb_root) {
		dir_name = "";
	} else {
		dir_name = inode_name((struct inode *)dir);
	}

	n = cdfs_find_dir(fsi, dir_name, strlen(dir_name));

	res = cdfs_find_in_dir(fsi, n, (char *)name, strlen(name), &rec);
	if (res) {
		return NULL;
	}

	res = cdfs_alloc_inode_priv(node);
	if (res) {
		return NULL;
	}

	res = cdfs_fill_node(node, (char *)name, fsi, rec);
	if (res) {
		return NULL;
	}

	return node;
}

struct super_block_operations cdfs_sbops = {
	.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = cdfs_destroy_inode,
};

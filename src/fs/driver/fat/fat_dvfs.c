/**
 * @file
 * @brief Implementation of FAT driver for DVFS
 *
 * @date   11 Apr 2015
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 */

#include <limits.h>
#include <stddef.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>

#include <fs/inode.h>
#include <fs/super_block.h>

#include "fat.h"

/* @brief Figure out if node at specific path exists or not
 * @note  Assume dir is root
 * @note IMPORTANT: this functions should not be calls in the middle of iterate,
 * as it wipes dirinfo content
 *
 * @param name Full path to the extected node
 * @param dir  Inode of corresponding parent directory
 *
 * @return Pointer of inode or NULL if not found
 */
struct inode *fat_ilookup(char const *name, struct inode const *dir) {
	struct dirinfo *di;
	struct fat_dirent de;
	struct super_block *sb;
	uint8_t tmp_ent;
	uint8_t tmp_sec;
	uint32_t tmp_clus;
	struct inode *node;
	char tmppath[128];
	int found = 0;

	assert(name);
	assert(S_ISDIR(dir->i_mode));

	sb = dir->i_sb;
	di = inode_priv(dir);

	assert(di);

	tmp_ent = di->currententry;
	tmp_sec = di->currentsector;
	tmp_clus = di->currentcluster;
	fat_reset_dir(di);

	if (read_dir_buf(di)) {
		goto err_out;
	}

	while (!fat_get_next_long(di, &de, tmppath)) {
		if (!strncmp(tmppath, name, NAME_MAX - 1)) {
			found = 1;
			break;
		}
	}

	if (!found)
		goto err_out;

	if (NULL == (node = dvfs_alloc_inode(sb)))
		goto err_out;

	if (fat_fill_inode(node, &de, di))
		goto err_out;

	goto succ_out;
err_out:
	node = NULL;
succ_out:
	di->currentcluster = tmp_clus;
	di->currententry = tmp_ent;
	di->currentsector = tmp_sec;
	return node;
}

extern struct idesc *dvfs_file_open_idesc(struct lookup *lookup, int __oflag);
extern int fat_destroy_inode(struct inode *inode);
struct super_block_operations fat_sbops = {
	.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = fat_destroy_inode,
};

/**
 * @file
 * @brief Implementation of FAT driver for DVFS
 *
 * @date   11 Apr 2015
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>

#include <drivers/device.h>

#include <fs/dvfs.h>
#include <util/math.h>
#include <util/log.h>

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
static struct inode *fat_ilookup(char const *name, struct inode const *dir) {
	struct dirinfo *di;
	struct fat_dirent de;
	struct super_block *sb;
	uint8_t tmp_ent;
	uint8_t tmp_sec;
	uint32_t tmp_clus;
	struct inode *node;
	char tmppath[PATH_MAX];
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
		if (!strncmp(tmppath, name, sizeof(tmppath))) {
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

extern int fat_create(struct inode *i_new, struct inode *i_dir, int mode);
/* Declaration of operations */
struct inode_operations fat_iops = {
	.create   = fat_create,
	.lookup   = fat_ilookup,
	.remove   = fat_delete,
	.iterate  = fat_iterate,
	.truncate = fat_truncate,
};

extern struct file_operations fat_fops;

static int fat_destroy_inode(struct inode *inode) {
	struct fat_file_info *fi;
	struct dirinfo *di;

	if (!inode->i_data)
		return 0;

	if (S_ISDIR(inode->i_mode)) {
		di = inode->i_data;
		fat_dirinfo_free(di);
	} else {
		fi = inode->i_data;
		fat_file_free(fi);
	}

	return 0;
}

struct super_block_operations fat_sbops = {
	.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = fat_destroy_inode,
};

extern int fat_create(struct inode *i_new, struct inode *i_dir, int mode);
extern int fat_fill_sb(struct super_block *sb, const char *source);
extern int fat_clean_sb(struct super_block *sb);
extern int fat_format(struct block_dev *dev, void *priv);
static const struct fs_driver dfs_fat_driver = {
	.name      = "vfat",
	.fill_sb   = fat_fill_sb,
	.format    = fat_format,
	.clean_sb  = fat_clean_sb,
};

ARRAY_SPREAD_DECLARE(const struct fs_driver *const, fs_drivers_registry);
ARRAY_SPREAD_ADD(fs_drivers_registry, &dfs_fat_driver);

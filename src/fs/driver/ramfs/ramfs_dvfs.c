/**
 * @file
 * @brief Tmp file system
 *
 * @date 12.11.12
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include <util/array.h>
#include <util/indexator.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h> /* PAGE_SIZE() */
#include <mem/page.h>

#include <fs/file_desc.h>
#include <fs/dvfs.h>

#include <util/math.h>
#include <util/err.h>

#include <embox/unit.h>
#include <drivers/block_dev.h>
#include <drivers/block_dev/ramdisk/ramdisk.h>
#include "ramfs.h"

static int ramfs_iterate(struct inode *next, struct inode *parent, struct dir_ctx *ctx) {
	struct ramfs_fs_info *fsi;
	int cur_id;

	assert(ctx);
	assert(next);
	assert(parent);
	assert(parent->i_sb);

	cur_id = (int) ctx->fs_ctx;
	fsi = parent->i_sb->sb_data;

	while (cur_id < RAMFS_FILES) {
		if (ramfs_files[cur_id].fsi != fsi) {
			cur_id++;
			continue;
		}

		next->i_data = &ramfs_files[cur_id];
		next->i_no = cur_id;
		ctx->fs_ctx = (void *) (cur_id + 1);
		return 0;
	}

	ctx->fs_ctx = NULL;
	return -1;
}

static int ramfs_create(struct inode *i_new, struct inode *i_dir, int mode) {
	struct ramfs_file_info *fi;
	size_t fi_index;

	assert(i_new);
	assert(i_new->i_dentry);
	assert(i_new->i_dentry->name);

	fi = ramfs_file_alloc(i_new);

	strncpy(fi->name, i_new->i_dentry->name, sizeof(fi->name) - 1);

	i_new->i_no = fi->index;

	return 0;
}

static struct inode *ramfs_ilookup(char const *name, struct inode const *dir) {
	struct inode *node;
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

		if (NULL == (node = dvfs_alloc_inode(sb))) {
			return NULL;
		}

		node->i_data = &ramfs_files[i];
		node->i_no = ramfs_files[i].index;

		return node;
	}

	return NULL;
}

/* Declaration of operations */
struct inode_operations ramfs_iops = {
	.create   = ramfs_create,
	.lookup   = ramfs_ilookup,
	.remove   = ramfs_delete,
	.iterate  = ramfs_iterate,
	.truncate = ramfs_truncate,
};

static int ramfs_destroy_inode(struct inode *inode) {
	struct ramfs_file_info *fi;

	assert(inode);

	fi = inode->i_data;
	if (fi) {
		index_free(&ramfs_file_idx, fi->index);
	}

	return 0;
}

struct super_block_operations ramfs_sbops = {
	.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = ramfs_destroy_inode,
};

static const struct fs_driver ramfs_dumb_driver = {
	.name      = "ramfs",
	.fill_sb   = ramfs_fill_sb,
	.format    = ramfs_format,
};

ARRAY_SPREAD_DECLARE(const struct fs_driver *const, fs_drivers_registry);
ARRAY_SPREAD_ADD(fs_drivers_registry, &ramfs_dumb_driver);

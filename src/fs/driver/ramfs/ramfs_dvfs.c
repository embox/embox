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
#include <sys/types.h>

#include <lib/libds/array.h>
#include <lib/libds/indexator.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h> /* PAGE_SIZE() */
#include <mem/page.h>

#include <fs/file_desc.h>
#include <fs/dvfs.h>

#include <util/math.h>
#include <util/err.h>

#include <embox/unit.h>
#include <drivers/block_dev.h>

#include "ramfs.h"

extern struct ramfs_file_info ramfs_files[RAMFS_FILES];
extern int ramfs_iterate(struct inode *next, char *name, struct inode *parent, struct dir_ctx *ctx);

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

		node->i_privdata = &ramfs_files[i];
		node->i_no = ramfs_files[i].index;
		node->i_size = ramfs_files[i].length;
		node->i_mode = ramfs_files[i].mode & (S_IFMT | S_IRWXA);

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


struct super_block_operations ramfs_sbops = {
	.open_idesc    = dvfs_file_open_idesc,
	.destroy_inode = ramfs_destroy_inode,
};

static const struct fs_driver ramfs_dumb_driver = {
	.name      = "ramfs",
	.fill_sb   = ramfs_fill_sb,
	.format    = ramfs_format,
};

DECLARE_FILE_SYSTEM_DRIVER(ramfs_dumb_driver);

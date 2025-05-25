/**
 * @file
 * @brief ext3 file system
 *
 * @date 12.03.2013
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <string.h>
#include <sys/stat.h>
 
#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/ext2.h>
#include <fs/hlpr_path.h>
#include <fs/dir_context.h>
#include <fs/inode_operation.h>

#include "ext3_journal.h"


static int ext3fs_create(struct inode *node, struct inode *parent_node, int mode);

static int ext3fs_truncate(struct inode *node, off_t length);

extern struct inode_operations ext2_iops;

static int ext3fs_create(struct inode *node, struct inode *parent_node, int mode) {
	struct ext2_fs_info *fsi;
	journal_handle_t *handle;
	int res = -1;

	fsi = parent_node->i_sb->sb_data;
	/**
	 * ext3_trans_blocks(1) - to modify parent_node's data block
	 * 2 blocks for child = 1 inode + 1 inode bitmap.
	 * 2 * (ext3_trans_blocks(1) + 2) blocks to create "." and ".."
	 */
	if (!(handle = journal_start(fsi->journal, 3 * (ext3_trans_blocks(1) + 2)))) {
		return -1;
	}
	res = ext2_iops.ino_create(node, parent_node, node->i_mode);
	journal_stop(handle);

	return res;
}

static int ext3fs_delete(struct inode *dir, struct inode *node) {
	struct ext2_fs_info *fsi;
	journal_handle_t *handle;
	int res;

	fsi = node->i_sb->sb_data;
	/**
	 * Same as in ext3fs_create:
	 * ext3_trans_blocks(1) - to modify parent_node's data block
	 * 2 blocks for child = 1 inode + 1 inode bitmap
	 */
	if (!(handle = journal_start(fsi->journal, ext3_trans_blocks(1) + 2))) {
		return -1;
	}
	res = ext2_iops.ino_remove(dir, node);
	journal_stop(handle);

	return res;
}


/* TODO ext2fs_truncate()? */
static int ext3fs_truncate (struct inode *node, off_t length) {

	inode_size_set(node, length);

	return 0;
}

extern int ext2_iterate(struct inode *next, char *next_name, struct inode *parent, struct dir_ctx *dir_ctx);
extern struct inode *_ext2fs_lookup(struct inode *node, char const *name, struct inode const *dir);
struct inode_operations ext3_iops = {
	.ino_create       = ext3fs_create,
	.ino_remove       = ext3fs_delete,
	.ino_iterate      = ext2_iterate,
	.ino_lookup       = _ext2fs_lookup,
	.ino_truncate     = ext3fs_truncate, /* TODO journaling */

	.ino_getxattr     = ext2fs_getxattr,
	.ino_setxattr     = ext2fs_setxattr, /* TODO journaling */
	.ino_listxattr    = ext2fs_listxattr,
};


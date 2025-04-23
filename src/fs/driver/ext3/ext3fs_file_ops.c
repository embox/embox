/**
 * @file
 * @brief ext3 file system
 *
 * @date 12.03.2013
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stddef.h>

#include <fs/inode.h>
#include <fs/super_block.h>
#include <fs/ext2.h>
 
#include <fs/file_desc.h>

#include <util/err.h>

#include "ext3_journal.h"

/* file operations */
static struct idesc *ext3fs_open(struct inode *node, struct idesc *idesc, int __oflag);
static int ext3fs_close(struct file_desc *desc);
static size_t ext3fs_read(struct file_desc *desc, void *buf, size_t size);
static size_t ext3fs_write(struct file_desc *desc, void *buf, size_t size);

extern struct file_operations ext2_fop;
/*
 * file_operation
 */
static struct idesc *ext3fs_open(struct inode *node, struct idesc *idesc, int __oflag) {
	return ext2_fop.open(node, idesc, __oflag);
}

static int ext3fs_close(struct file_desc *desc) {
	return ext2_fop.close(desc);
}

static size_t ext3fs_read(struct file_desc *desc, void *buff, size_t size) {
	return ext2_fop.read(desc, buff, size);
}

static size_t ext3fs_write(struct file_desc *desc, void *buff, size_t size) {
	int res;
	size_t datablocks;
	struct ext2_fs_info *fsi;
	journal_handle_t *handle;

	assert(desc->f_inode);
	fsi = desc->f_inode->i_sb->sb_data;
	/* N * SECTOR_SIZE + K bytes of data can dirty N + 2 only if K >= 2  */
	datablocks = (size + SECTOR_SIZE - 2) / SECTOR_SIZE + 1;
	/* TODO recalculate */
	if (!(handle = journal_start(fsi->journal, 4 * ext3_trans_blocks(datablocks)))) {
		return -1;
	}
	res = ext2_fop.write(desc, buff, size);
	journal_stop(handle);

	return res;
}

struct file_operations ext3_fop = {
	.open = ext3fs_open,
	.close = ext3fs_close,
	.read = ext3fs_read,
	.write = ext3fs_write,
};

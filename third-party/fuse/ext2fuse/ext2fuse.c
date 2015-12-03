/**
 * @file
 * @brief ext2fuse support
 *
 * @date 16.11.2015
 * @author Alexander Kalmuk
 */

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdarg.h>
#include <stdlib.h>

#include <fs/dvfs.h>
#include <kernel/printk.h>
#include <embox/unit.h>

// Allocates embox-specific requests for the FUSE
#include <fuse_req_alloc.h>

// Needed by fuse_common.h
#define _FILE_OFFSET_BITS 64

// it is from fuse-ext2fs.c
#define FUSE_USE_VERSION 25
#include <fuse_lowlevel.h>
#include <fuse_opt.h>

extern struct fuse_lowlevel_ops *ext2fs_register(void);

static struct fuse_lowlevel_ops *ext2fuse_ops;

static struct idesc *ext2fuse_open(struct inode *node, struct idesc *desc) {
	struct fuse_file_info *fi;

	fi = node->i_data;
	assert(fi);
	ext2fuse_ops->open(NULL, node->i_no, fi);

	return desc;
}

static size_t ext2fuse_read(struct file *desc, void *buf, size_t size) {
	struct inode *inode;
	struct fuse_file_info *fi;
	struct fuse_req_embox *req;
	size_t ret;

	if (NULL == (req = fuse_req_alloc())) {
		return 0;
	}

	inode = desc->f_inode;
	if (size > inode->length - desc->pos) {
		size = inode->length - desc->pos;
	}
	fi = inode->i_data;
	req->buf = buf;
	ext2fuse_ops->read((fuse_req_t) req, inode->i_no, size, desc->pos, fi);
	memcpy(buf, req->buf, req->buf_size);
	ret = req->buf_size;
	fuse_req_free(req);

	return ret;
}

static struct inode *ext2fuse_lookup(char const *name, struct dentry const *dir) {
	struct inode *node;
	struct fuse_file_info *fi;
	struct fuse_req_embox *req;

	if (NULL == (node = dvfs_alloc_inode(dir->d_sb))) {
		return NULL;
	}
	if (NULL == (fi = malloc(sizeof *fi))) {
		return NULL;
	}
	if (NULL == (req = fuse_req_alloc())) {
		return NULL;
	}

	node->i_data = fi;
	req->node = node;
	ext2fuse_ops->lookup((fuse_req_t) req, dir->d_inode->i_no, name);
	fuse_req_free(req);

	return node;
}

static int ext2fuse_iterate(struct inode *next, struct inode *parent, struct dir_ctx *ctx) {
	char buf[512];
	struct fuse_req_embox *req;

	if (NULL == (req = fuse_req_alloc())) {
		return -1;
	}
	req->node = parent;
	req->buf = buf;
	ext2fuse_ops->readdir((fuse_req_t) req, parent->i_no, 512, 0, parent->i_data);
	fuse_req_free(req);

	return -1;
}

static int ext2fuse_pathname(struct inode *inode, char *buf, int flags) {
	return 0;
}

static int ext2fuse_mount_end(struct super_block *sb) {
	return 0;
}

static int ext2fuse_destroy_inode(struct inode *inode) {
	return 0;
}

struct super_block_operations ext2fuse_sbops = {
	.open_idesc = dvfs_file_open_idesc,
	.destroy_inode = ext2fuse_destroy_inode,
};

struct inode_operations ext2fuse_iops = {
	.lookup   = ext2fuse_lookup,
	.iterate  = ext2fuse_iterate,
	.pathname = ext2fuse_pathname,
};

struct file_operations ext2fuse_fops = {
	.open = ext2fuse_open,
	.read  = ext2fuse_read
};

static int ext2fuse_fill_sb(struct super_block *sb, struct file *bdev_file) {
	sb->sb_iops = &ext2fuse_iops;
	sb->sb_fops = &ext2fuse_fops;
	sb->sb_ops  = &ext2fuse_sbops;
	sb->bdev = NULL;

	ext2fuse_ops = ext2fs_register();
	// SHOULD BE ext2fuse_ops->init(bdev_file->f_dentry->name);
	ext2fuse_ops->init("/dev/hda");

	if (bdev_file) {
		return -1;
	}

	return 0;
}

static struct dumb_fs_driver ext2fuse_dumb_driver = {
	.name      = "ext2fuse",
	.fill_sb   = ext2fuse_fill_sb,
	.mount_end = ext2fuse_mount_end,
};

ARRAY_SPREAD_DECLARE(struct dumb_fs_driver *, dumb_drv_tab);
ARRAY_SPREAD_ADD(dumb_drv_tab, &ext2fuse_dumb_driver);

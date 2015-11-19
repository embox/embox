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

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>

#include <kernel/printk.h>
#include <embox/unit.h>

// Needed by fuse_common.h
#define _FILE_OFFSET_BITS 64

// it is from fuse-ext2fs.c
#define FUSE_USE_VERSION 25
#include <fuse_lowlevel.h>
#include <fuse_opt.h>

extern struct fuse_lowlevel_ops *ext2fs_register(void);

static struct fuse_lowlevel_ops *ext2fuse_ops;

static int ext2fuse_open(struct node *nod, struct file_desc *desc, int flags) {
	return 0;
}

static int ext2fuse_close(struct file_desc *desc) {
	return 0;
}

static size_t ext2fuse_read(struct file_desc *desc, void *buf, size_t size) {
	return 0;
}

static int ext2fuse_mount(void *dev, void *dir) {
	ext2fuse_ops = ext2fs_register();

	return 0;
}

static struct kfile_operations ext2fuse_fop = {
	.open = ext2fuse_open,
	.close = ext2fuse_close,
	.read = ext2fuse_read,
};

static struct fsop_desc ext2fuse_fsop = {
	.mount = ext2fuse_mount,
};

static struct fs_driver ext2fuse_driver = {
	.name = "ext2fuse",
	.file_op = &ext2fuse_fop,
	.fsop = &ext2fuse_fsop,
	.mount_dev_by_string = true,
};

DECLARE_FILE_SYSTEM_DRIVER(ext2fuse_driver);

/**
 * @file
 * @brief ffs file system
 *
 * @date 14.03.2013
 * @author Andrey Gazukin
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include <fs/fs_driver.h>
#include <fs/vfs.h>
#include <fs/hlpr_path.h>
#include <fs/file_operation.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>

#include <util/array.h>
#include <embox/unit.h>
#include <embox/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>


#define PSEVDOFS_NAME "vfat"
#define FS_NAME "ffs"


static int ffsfs_open(struct node *node, struct file_desc *file_desc,
		int flags);
static int ffsfs_close(struct file_desc *desc);
static size_t ffsfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t ffsfs_write(struct file_desc *desc, void *buf, size_t size);
static int ffsfs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations ffs_fop = {
	.open = ffsfs_open,
	.close = ffsfs_close,
	.read = ffsfs_read,
	.write = ffsfs_write,
	.ioctl = ffsfs_ioctl,
};

/*
 * file_operation
 */
static int ffsfs_open(struct node *node, struct file_desc *desc, int flags) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->open(node, desc, flags);
}

static int ffsfs_close(struct file_desc *desc) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->close(desc);
}

static size_t ffsfs_read(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->read(desc, buff, size);
}

static size_t ffsfs_write(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->write(desc, buff, size);
}

static int ffsfs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

static int ffsfs_init(void * par);
static int ffsfs_format(void *path);
static int ffsfs_mount(void *dev, void *dir);
static int ffsfs_create(struct node *parent_node, struct node *node);
static int ffsfs_delete(struct node *node);
static int ffsfs_truncate(struct node *node, off_t length);
static int ffsfs_umount(void *dir);


static struct fsop_desc ffs_fsop = {
	.init	     = ffsfs_init,
	.format	     = ffsfs_format,
	.mount	     = ffsfs_mount,
	.create_node = ffsfs_create,
	.delete_node = ffsfs_delete,

	.getxattr    = NULL,
	.setxattr    = NULL,
	.listxattr   = NULL,

	.truncate    = ffsfs_truncate,
	.umount      = ffsfs_umount,
};

static int ffsfs_init(void * par) {

	return 0;
};

static struct fs_driver ffsfs_driver = {
	.name = FS_NAME,
	.file_op = &ffs_fop,
	.fsop = &ffs_fsop,
};

static int ffsfs_create(struct node *parent_node, struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->create_node(parent_node, node);
}

static int ffsfs_delete(struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->delete_node(node);
}

static int ffsfs_format(void *dev) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->format(dev);
}

static int ffsfs_mount(void *dev, void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->mount(dev, dir);
}

static int ffsfs_truncate (struct node *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	return 0;
}

static int ffsfs_umount(void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->umount(dir);
}

DECLARE_FILE_SYSTEM_DRIVER(ffsfs_driver);

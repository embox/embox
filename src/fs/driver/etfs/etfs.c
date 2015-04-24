/**
 * @file
 * @brief etfs file system
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
#include <util/array.h>
#include <embox/unit.h>
#include <embox/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>

#include <fs/file_system.h>
#include <fs/file_desc.h>

#define PSEVDOFS_NAME "vfat"
#define FS_NAME "etfs"


static int etfsfs_open(struct node *node, struct file_desc *file_desc,
		int flags);
static int etfsfs_close(struct file_desc *desc);
static size_t etfsfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t etfsfs_write(struct file_desc *desc, void *buf, size_t size);
static int etfsfs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations etfs_fop = {
	.open = etfsfs_open,
	.close = etfsfs_close,
	.read = etfsfs_read,
	.write = etfsfs_write,
	.ioctl = etfsfs_ioctl,
};

/*
 * file_operation
 */
static int etfsfs_open(struct node *node, struct file_desc *desc, int flags) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->open(node, desc, flags);
}

static int etfsfs_close(struct file_desc *desc) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->close(desc);
}

static size_t etfsfs_read(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->read(desc, buff, size);
}

static size_t etfsfs_write(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->write(desc, buff, size);
}

static int etfsfs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

static int etfsfs_init(void * par);
static int etfsfs_format(void *path);
static int etfsfs_mount(void *dev, void *dir);
static int etfsfs_create(struct node *parent_node, struct node *node);
static int etfsfs_delete(struct node *node);
static int etfsfs_truncate(struct node *node, off_t length);
static int etfsfs_umount(void *dir);


static struct fsop_desc etfs_fsop = {
	.init	     = etfsfs_init,
	.format	     = etfsfs_format,
	.mount	     = etfsfs_mount,
	.create_node = etfsfs_create,
	.delete_node = etfsfs_delete,

	.getxattr    = NULL,
	.setxattr    = NULL,
	.listxattr   = NULL,

	.truncate    = etfsfs_truncate,
	.umount      = etfsfs_umount,
};

static int etfsfs_init(void * par) {

	return 0;
};

static struct fs_driver etfsfs_driver = {
	.name = FS_NAME,
	.file_op = &etfs_fop,
	.fsop = &etfs_fsop,
};

static int etfsfs_create(struct node *parent_node, struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->create_node(parent_node, node);
}

static int etfsfs_delete(struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->delete_node(node);
}

static int etfsfs_format(void *dev) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->format(dev);
}

static int etfsfs_mount(void *dev, void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->mount(dev, dir);
}

static int etfsfs_truncate (struct node *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	return 0;
}

static int etfsfs_umount(void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->umount(dir);
}

DECLARE_FILE_SYSTEM_DRIVER(etfsfs_driver);

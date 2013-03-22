/**
 * @file
 * @brief cifs file system
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
#include <fs/fat.h>
#include <fs/path.h>
#include <util/array.h>
#include <embox/unit.h>
#include <embox/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>
//#include <drivers/ramdisk.h>
#include <fs/file_system.h>
#include <fs/file_desc.h>

#define PSEVDOFS_NAME "vfat"
#define FS_NAME "cifs"


static int cifsfs_open(struct node *node, struct file_desc *file_desc,
		int flags);
static int cifsfs_close(struct file_desc *desc);
static size_t cifsfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t cifsfs_write(struct file_desc *desc, void *buf, size_t size);
static int cifsfs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations cifs_fop = {
	.open = cifsfs_open,
	.close = cifsfs_close,
	.read = cifsfs_read,
	.write = cifsfs_write,
	.ioctl = cifsfs_ioctl,
};

/*
 * file_operation
 */
static int cifsfs_open(struct node *node, struct file_desc *desc, int flags) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->open(node, desc, flags);
}

static int cifsfs_close(struct file_desc *desc) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->close(desc);
}

static size_t cifsfs_read(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->read(desc, buff, size);
}

static size_t cifsfs_write(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->write(desc, buff, size);
}

static int cifsfs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

static int cifsfs_init(void * par);
static int cifsfs_format(void *path);
static int cifsfs_mount(void *dev, void *dir);
static int cifsfs_create(struct node *parent_node, struct node *node);
static int cifsfs_delete(struct node *node);
static int cifsfs_truncate(struct node *node, off_t length);
static int cifsfs_umount(void *dir);


static struct fsop_desc cifs_fsop = {
	.init	     = cifsfs_init,
	.format	     = cifsfs_format,
	.mount	     = cifsfs_mount,
	.create_node = cifsfs_create,
	.delete_node = cifsfs_delete,

	.getxattr    = NULL,
	.setxattr    = NULL,
	.listxattr   = NULL,

	.truncate    = cifsfs_truncate,
	.umount      = cifsfs_umount,
};

static int cifsfs_init(void * par) {

	return 0;
};

static struct fs_driver cifsfs_driver = {
	.name = FS_NAME,
	.file_op = &cifs_fop,
	.fsop = &cifs_fsop,
};

static int cifsfs_create(struct node *parent_node, struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->create_node(parent_node, node);
}

static int cifsfs_delete(struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->delete_node(node);
}

static int cifsfs_format(void *dev) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->format(dev);
}

static int cifsfs_mount(void *dev, void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->mount(dev, dir);
}

static int cifsfs_truncate (struct node *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	return 0;
}

static int cifsfs_umount(void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->umount(dir);
}

DECLARE_FILE_SYSTEM_DRIVER(cifsfs_driver);

/**
 * @file
 * @brief ramfs file system
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
#define FS_NAME "ramfs"


static int ramfsfs_open(struct node *node, struct file_desc *file_desc,
		int flags);
static int ramfsfs_close(struct file_desc *desc);
static size_t ramfsfs_read(struct file_desc *desc, void *buf, size_t size);
static size_t ramfsfs_write(struct file_desc *desc, void *buf, size_t size);
static int ramfsfs_ioctl(struct file_desc *desc, int request, va_list args);

static struct kfile_operations ramfs_fop = {
	.open = ramfsfs_open,
	.close = ramfsfs_close,
	.read = ramfsfs_read,
	.write = ramfsfs_write,
	.ioctl = ramfsfs_ioctl,
};

/*
 * file_operation
 */
static int ramfsfs_open(struct node *node, struct file_desc *desc, int flags) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->open(node, desc, flags);
}

static int ramfsfs_close(struct file_desc *desc) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->close(desc);
}

static size_t ramfsfs_read(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->read(desc, buff, size);
}

static size_t ramfsfs_write(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->write(desc, buff, size);
}

static int ramfsfs_ioctl(struct file_desc *desc, int request, va_list args) {
	return 0;
}

static int ramfsfs_init(void * par);
static int ramfsfs_format(void *path);
static int ramfsfs_mount(void *dev, void *dir);
static int ramfsfs_create(struct node *parent_node, struct node *node);
static int ramfsfs_delete(struct node *node);
static int ramfsfs_truncate(struct node *node, off_t length);
static int ramfsfs_umount(void *dir);


static struct fsop_desc ramfs_fsop = {
	.init	     = ramfsfs_init,
	.format	     = ramfsfs_format,
	.mount	     = ramfsfs_mount,
	.create_node = ramfsfs_create,
	.delete_node = ramfsfs_delete,

	.getxattr    = NULL,
	.setxattr    = NULL,
	.listxattr   = NULL,

	.truncate    = ramfsfs_truncate,
	.umount      = ramfsfs_umount,
};

static int ramfsfs_init(void * par) {

	return 0;
};

static struct fs_driver ramfsfs_driver = {
	.name = FS_NAME,
	.file_op = &ramfs_fop,
	.fsop = &ramfs_fsop,
};

static int ramfsfs_create(struct node *parent_node, struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->create_node(parent_node, node);
}

static int ramfsfs_delete(struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->delete_node(node);
}

static int ramfsfs_format(void *dev) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->format(dev);
}

static int ramfsfs_mount(void *dev, void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->mount(dev, dir);
}

static int ramfsfs_truncate (struct node *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	return 0;
}

static int ramfsfs_umount(void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->umount(dir);
}

DECLARE_FILE_SYSTEM_DRIVER(ramfsfs_driver);

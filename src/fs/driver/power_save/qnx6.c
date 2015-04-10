/**
 * @file
 * @brief qnx6 file system
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
#include <fs/file_operation.h>
#include <fs/vfs.h>
#include <fs/hlpr_path.h>
#include <util/array.h>
#include <embox/unit.h>
#include <embox/block_dev.h>
#include <mem/misc/pool.h>
#include <mem/phymem.h>

#include <fs/file_system.h>
#include <fs/file_desc.h>

#define PSEVDOFS_NAME "vfat"
#define FS_NAME "qnx6"


static int qnx6fs_open(struct node *node, struct file_desc *file_desc,
		int flags);
static int qnx6fs_close(struct file_desc *desc);
static size_t qnx6fs_read(struct file_desc *desc, void *buf, size_t size);
static size_t qnx6fs_write(struct file_desc *desc, void *buf, size_t size);
static int qnx6fs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations qnx6_fop = {
	.open = qnx6fs_open,
	.close = qnx6fs_close,
	.read = qnx6fs_read,
	.write = qnx6fs_write,
	.ioctl = qnx6fs_ioctl,
};

/*
 * file_operation
 */
static int qnx6fs_open(struct node *node, struct file_desc *desc, int flags) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->open(node, desc, flags);
}

static int qnx6fs_close(struct file_desc *desc) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->close(desc);
}

static size_t qnx6fs_read(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->read(desc, buff, size);
}

static size_t qnx6fs_write(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->write(desc, buff, size);
}

static int qnx6fs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

static int qnx6fs_init(void * par);
static int qnx6fs_format(void *path);
static int qnx6fs_mount(void *dev, void *dir);
static int qnx6fs_create(struct node *parent_node, struct node *node);
static int qnx6fs_delete(struct node *node);
static int qnx6fs_truncate(struct node *node, off_t length);
static int qnx6fs_umount(void *dir);


static struct fsop_desc qnx6_fsop = {
	.init	     = qnx6fs_init,
	.format	     = qnx6fs_format,
	.mount	     = qnx6fs_mount,
	.create_node = qnx6fs_create,
	.delete_node = qnx6fs_delete,

	.getxattr    = NULL,
	.setxattr    = NULL,
	.listxattr   = NULL,

	.truncate    = qnx6fs_truncate,
	.umount      = qnx6fs_umount,
};

static int qnx6fs_init(void * par) {

	return 0;
};

static struct fs_driver qnx6fs_driver = {
	.name = FS_NAME,
	.file_op = &qnx6_fop,
	.fsop = &qnx6_fsop,
};

static int qnx6fs_create(struct node *parent_node, struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->create_node(parent_node, node);
}

static int qnx6fs_delete(struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->delete_node(node);
}

static int qnx6fs_format(void *dev) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->format(dev);
}

static int qnx6fs_mount(void *dev, void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->mount(dev, dir);
}

static int qnx6fs_truncate (struct node *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	return 0;
}

static int qnx6fs_umount(void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->umount(dir);
}

DECLARE_FILE_SYSTEM_DRIVER(qnx6fs_driver);

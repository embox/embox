/**
 * @file
 * @brief jffs2 file system
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
#define FS_NAME "jffs2"


static int jffs2fs_open(struct node *node, struct file_desc *file_desc,
		int flags);
static int jffs2fs_close(struct file_desc *desc);
static size_t jffs2fs_read(struct file_desc *desc, void *buf, size_t size);
static size_t jffs2fs_write(struct file_desc *desc, void *buf, size_t size);
static int jffs2fs_ioctl(struct file_desc *desc, int request, ...);

static struct kfile_operations jffs2_fop = {
	.open = jffs2fs_open,
	.close = jffs2fs_close,
	.read = jffs2fs_read,
	.write = jffs2fs_write,
	.ioctl = jffs2fs_ioctl,
};

/*
 * file_operation
 */
static int jffs2fs_open(struct node *node, struct file_desc *desc, int flags) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->open(node, desc, flags);
}

static int jffs2fs_close(struct file_desc *desc) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->close(desc);
}

static size_t jffs2fs_read(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->read(desc, buff, size);
}

static size_t jffs2fs_write(struct file_desc *desc, void *buff, size_t size) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->file_op->write(desc, buff, size);
}

static int jffs2fs_ioctl(struct file_desc *desc, int request, ...) {
	return 0;
}

static int jffs2fs_init(void * par);
static int jffs2fs_format(void *path);
static int jffs2fs_mount(void *dev, void *dir);
static int jffs2fs_create(struct node *parent_node, struct node *node);
static int jffs2fs_delete(struct node *node);
static int jffs2fs_truncate(struct node *node, off_t length);
static int jffs2fs_umount(void *dir);


static struct fsop_desc jffs2_fsop = {
	.init	     = jffs2fs_init,
	.format	     = jffs2fs_format,
	.mount	     = jffs2fs_mount,
	.create_node = jffs2fs_create,
	.delete_node = jffs2fs_delete,

	.getxattr    = NULL,
	.setxattr    = NULL,
	.listxattr   = NULL,

	.truncate    = jffs2fs_truncate,
	.umount      = jffs2fs_umount,
};

static int jffs2fs_init(void * par) {

	return 0;
};

static struct fs_driver jffs2fs_driver = {
	.name = FS_NAME,
	.file_op = &jffs2_fop,
	.fsop = &jffs2_fsop,
};

static int jffs2fs_create(struct node *parent_node, struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->create_node(parent_node, node);
}

static int jffs2fs_delete(struct node *node) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->delete_node(node);
}

static int jffs2fs_format(void *dev) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->format(dev);
}

static int jffs2fs_mount(void *dev, void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->mount(dev, dir);
}

static int jffs2fs_truncate (struct node *node, off_t length) {
	struct nas *nas = node->nas;

	nas->fi->ni.size = length;

	return 0;
}

static int jffs2fs_umount(void *dir) {
	struct fs_driver *drv;

	if(NULL == (drv = fs_driver_find_drv(PSEVDOFS_NAME))) {
		return -1;
	}

	return drv->fsop->umount(dir);
}

DECLARE_FILE_SYSTEM_DRIVER(jffs2fs_driver);

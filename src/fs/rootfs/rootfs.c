/**
 * @file
 * @details realize file operation function in line address space
 *
 * @date 29.06.2009
 * @author Anton Bondarev
 */
#include <string.h>
#include <fs/ramfs.h>
#include <fs/fs.h>
#include <linux/init.h>
#include <fs/node.h>

static void   *rootfs_open(const char *path, const char *mode);
static int     rootfs_close(void *file);
static size_t  rootfs_read(void *buf, size_t size, size_t count, void *file);
static size_t  rootfs_write(const void *buf, size_t size, size_t count, void *file);
static int     rootfs_seek(void *file, long offset, int whence);
static int rootfs_ioctl(void *file, int request, va_list args);

static file_operations_t rootfs_fop = {
	rootfs_open,
	rootfs_close,
	rootfs_read,
	rootfs_write,
	rootfs_seek,
	rootfs_ioctl,
};

static node_t *root_node;

static int rootfs_init(void * par) {
	root_node = alloc_node("/");

	return 0;
}

node_t *rootfs_get_node(void) {
	return root_node;
}

static int rootfs_create(void *params) {
	return 0;
}

static int rootfs_delete(const char *file_name) {
	return 0;
}

static int rootfs_mount(void *par) {
	file_system_driver_t *fsdrv;
	if(NULL != (fsdrv = find_filesystem("ramfs"))) {
		fsdrv->fsop->mount(NULL);
	}
	if(NULL != (fsdrv = find_filesystem("devfs"))) {
		fsdrv->fsop->mount(NULL);
	}
	return 0;
}

static fsop_desc_t rootfs_fsop = {
	rootfs_init,
	rootfs_create,
	rootfs_delete,
	rootfs_mount
};

static file_system_driver_t rootfs_drv = {
	"rootfs",
	&rootfs_fop,
	&rootfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(rootfs_drv);

static void *rootfs_open(const char *file_name, const char *mode) {
	return NULL;
}

static int rootfs_close(void * file) {
	return 0;
}

static size_t rootfs_read(void *buf, size_t size, size_t count, void *file) {
	return 0;
}

static size_t rootfs_write(const void *buf, size_t size, size_t count, void *file) {
	return 0;
}

static int rootfs_seek(void *file, long offset, int whence) {
	return 0;
}

static int rootfs_ioctl(void *file, int request, va_list args) {
	return 0;
}


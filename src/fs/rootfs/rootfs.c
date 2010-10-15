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

//static FILE_DESC fdesc[CONFIG_MAX_FILE_QUANTITY];

static void *rootfs_open(const char *path, const char *mode);
static int rootfs_close(void *file);
static size_t rootfs_read(void *buf, size_t size, size_t count, void *file);
static size_t rootfs_write(const void *buf, size_t size, size_t count, void *file);
static int rootfs_seek(void *file, long offset, int whence);

static file_operations_t rootfs_fop = {
	rootfs_open,
	rootfs_close,
	rootfs_read,
	rootfs_write,
	rootfs_seek,
//	NULL
};

static node_t *root_node;

static int rootfs_init(void) {
	root_node = alloc_node("/");
	INIT_LIST_HEAD(&root_node->leaves);
	INIT_LIST_HEAD(&root_node->neighbors);
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

static fsop_desc_t rootfs_fsop = {
	rootfs_init,
	rootfs_create,
	rootfs_delete
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

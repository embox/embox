/**
 * @file
 * @details realize file operation function
 *
 * @date 29.06.09
 * @author Anton Bondarev
 */

#include <string.h>
#include <assert.h>
#include <fs/ramfs.h>
#include <fs/fs.h>
#include <fs/node.h>
#include <fs/driver_registry.h>
#include <util/array.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

static node_t *root_node;

static int rootfs_init(void * par) {
	return 0;
}

node_t *rootfs_get_node(void) {
	assert(NULL != root_node);
	return root_node;
}

static int rootfs_create(void *params) {
	return 0;
}

static int rootfs_delete(const char *file_name) {
	return 0;
}

static int rootfs_mount(void *par) {
	fs_drv_t *fsdrv;
	if (NULL != (fsdrv = filesystem_find_drv("ramfs"))) {
		fsdrv->fsop->mount(NULL);
	}
	if (NULL != (fsdrv = filesystem_find_drv("devfs"))) {
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

static const fs_drv_t rootfs_drv = {
	"rootfs",
	NULL,
	&rootfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(rootfs_drv);

static int unit_init(void) {
	root_node = alloc_node("/");
	rootfs_mount(NULL);
	return 0;
}


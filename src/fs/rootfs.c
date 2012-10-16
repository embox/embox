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
#include <fs/fs_drv.h>
#include <fs/node.h>
#include <util/array.h>
#include <errno.h>
#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

static node_t *root_node;


node_t *rootfs_get_node(void) {
	assert(NULL != root_node);
	return root_node;
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

static int unit_init(void) {
	root_node = alloc_node("/");
	if (root_node == NULL) {
		return -ENOMEM;
	}
	rootfs_mount(NULL);
	return 0;
}

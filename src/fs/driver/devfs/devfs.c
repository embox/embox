/**
 * @file
 * @brief Device file system
 *
 * @date 14.10.10
 * @author Nikolay Korotky
 */

#include <fs/fs_drv.h>
#include <fs/vfs.h>

#include <embox/device.h>
#include <embox/block_dev.h>

static int devfs_init(void * par) {

	return 0;
}

static int devfs_mount(void *dev, void *dir) {
	struct node *nod;

	if (NULL != (nod = vfs_find_node(dev, NULL))) {
		/* we already initialized devfs */
		return 0;
	}

	if (NULL == (nod = vfs_add_path(dev, NULL))) {
		return -1;
	}
	nod->type = NODE_TYPE_DIRECTORY;
	nod->nas = NULL; /* this is virtual folder to add folder or file we just add node */


	char_devs_init();

	block_devs_init();

	return 0;
}

static fsop_desc_t devfs_fsop = { devfs_init, NULL, devfs_mount,
		NULL, NULL};

static const fs_drv_t devfs_drv = {
	.name = "devfs",
	.file_op = NULL,
	.fsop = &devfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(devfs_drv);

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

static int devfs_init(void *par) {
	return 0;
}

static int devfs_mount(void *dev, void *dir) {
	int ret;
	struct node *nod;

	nod = vfs_find_node(dev, NULL);
	if (nod != NULL) {
		/* we already initialized devfs */
		return 0;
	}

	nod = vfs_add_path(dev, NULL);
	if (nod == NULL) {
		return -1;
	}

	nod->type = NODE_TYPE_DIRECTORY;
	nod->nas = NULL; /* this is virtual folder to add folder or file we just add node */

	ret = char_dev_init_all();
	if (ret != 0) {
		return ret;
	}

	return block_devs_init();
}

static fsop_desc_t devfs_fsop = { devfs_init, NULL, devfs_mount,
		NULL, NULL};

static const fs_drv_t devfs_drv = {
	.name = "devfs",
	.file_op = NULL,
	.fsop = &devfs_fsop
};

DECLARE_FILE_SYSTEM_DRIVER(devfs_drv);

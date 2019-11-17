/**
 * @file
 * @brief
 *
 * @date 31.07.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <assert.h>
#include <libgen.h>

#include <mem/phymem.h>

#include <embox/unit.h>

#include <fs/vfs.h>
#include <fs/node.h>
#include <fs/file_desc.h>
#include <fs/bcache.h>
#include <fs/file_operation.h>
#include <fs/fs_driver.h>
#include <drivers/block_dev.h>

extern struct idesc_ops idesc_bdev_ops;

struct block_dev *block_dev_create(const char *path, const struct block_dev_driver *driver, void *privdata) {
	struct block_dev *bdev;
	struct dev_module *devmod;
	struct path node, root;
	struct nas *nas;
	struct node_fi *node_fi;
	char full_path[PATH_MAX];

	bdev = block_dev_create_common((char *)basename((char *)path), driver, privdata);
	if (NULL == bdev) {
		return NULL;
	}

	devmod = dev_module_create(bdev->name, NULL, NULL, &idesc_bdev_ops, bdev);
	bdev->dev_module = devmod;

	vfs_get_root_path(&root);

	/* XXX  Workaround for compatibility with DVFS.
	 * Drivers which are used in both vfs and dvfs
	 * currently use different namings:
	 *  block_dev_create(blk0, ...) for dvfs
	 *  block_dev_create(/dev/blk0, ...) for vfs.
	 * So in case if block_dev_create was called without /dev
	 * just prepend the path with /dev. */
	if (strncmp("/dev", path, 4)) {
		strcpy(full_path, "/dev/");
		strncat(full_path, path, PATH_MAX - strlen("/dev") - 1);
	} else {
		strncpy(full_path, path, PATH_MAX);
	}

	if (0 != vfs_create(&root, full_path, S_IFBLK | S_IRALL | S_IWALL, &node)) {
		block_dev_free(bdev);
		return NULL;
	}

	bdev->dev_vfs_info = node.node;

	nas = node.node->nas;
	node_fi = nas->fi;
	node_fi->privdata = bdev;
	//node_fi->ni.size = 0;/*TODO*/
	//node_fi->ni.mtime = 0;/*TODO*/

	return bdev;
}

int block_dev_destroy(void *dev) {
	struct block_dev *bdev;

	bdev = block_dev(dev);
	vfs_del_leaf(bdev->dev_vfs_info);
	block_dev_free(bdev);

	return 0;
}

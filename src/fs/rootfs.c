/**
 * @file
 * @details realize file operation function
 *
 * @date 29.06.09
 * @author Anton Bondarev
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <fs/sys/fsop.h>/* now mount declaration in this header */

#include <fs/fs_drv.h>
#include <fs/node.h>
#include <fs/vfs.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

static node_t *root_node;

static int rootfs_mount(char *dev, char *dir, char *fs_type) {
	fs_drv_t *fsdrv;

	/* mount dev filesystem */
	if (NULL != (fsdrv = fs_driver_find_drv("devfs"))) {
		fsdrv->fsop->mount("/dev", NULL);
	}

	return mount(dev, dir, fs_type);
}

static int unit_init(void) {
	char *dev, *dir, *fs_type;

	dev = OPTION_STRING_GET(src);
	dir = OPTION_STRING_GET(dst);
	fs_type = OPTION_STRING_GET(fstype);

	if (0 != *dir) {
		root_node = vfs_get_root();
		if (root_node == NULL) {
			return -ENOMEM;
		}

		return rootfs_mount(dev, dir, fs_type);
	}
	return -ENOENT;
}

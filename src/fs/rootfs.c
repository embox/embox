/**
 * @file
 * @details realize file operation function
 *
 * @date 29.06.09
 * @author Anton Bondarev
 * @author Andrey Gazukin
 */

#include <errno.h>

#include <fs/sys/fsop.h> /* mount */
#include <fs/fs_driver.h>
#include <fs/node.h>
#include <fs/vfs.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

static int rootfs_mount(const char *dev, const char *dir, const char *fs_type) {
	struct fs_driver *fsdrv;

	/* mount dev filesystem */
	fsdrv = fs_driver_find_drv("devfs");
	if (fsdrv) {
		fsdrv->fsop->mount("/dev", NULL);
	}

	return mount((char *) dev, (char *) dir, (char *) fs_type);
}

static int unit_init(void) {
	const char *dev, *dir, *fs_type;

	dev = OPTION_STRING_GET(src);
	dir = OPTION_STRING_GET(dst);
	fs_type = OPTION_STRING_GET(fstype);

	if (!*dir) {
		return -ENOENT;
	}

#if 0
	root_node = vfs_get_root();
	if (root_node == NULL) {
		return -ENOMEM;
	}
#endif

	return rootfs_mount(dev, dir, fs_type);
}

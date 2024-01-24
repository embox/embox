/**
 * @file
 * @details realize file operation function
 *
 * @date 29.06.09
 * @author Anton Bondarev
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <string.h>
#include <sys/stat.h>

#include <lib/libds/array.h>

#include <fs/mount.h> /* mount */
#include <fs/fs_driver.h>
#include <fs/vfs.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

ARRAY_SPREAD_DEF(const struct auto_mount *const, auto_mount_tab);

static int rootfs_mount(const char *dev, const char *fs_type) {
	struct path node, root;
	mode_t mode;
	const struct auto_mount *auto_mnt;

	if (-1 == mount((char *) dev, "/", (char *) fs_type)) {
		return -errno;
	}

	mode = S_IFDIR | S_IRALL | S_IWALL | S_IXALL;
	vfs_get_root_path(&root);

	array_spread_foreach(auto_mnt, auto_mount_tab) {
		if (0 != vfs_create(&root, auto_mnt->mount_path, mode, &node)) {
			return -1;
		}

		if (-1 == mount(NULL, auto_mnt->mount_path, auto_mnt->fs_driver->name)) {
			return -errno;
		}
	}

	return 0;
}

static int unit_init(void) {
	const char *dev, *fs_type;

	dev = OPTION_STRING_GET(bdev);
	fs_type = OPTION_STRING_GET(fstype);

	return rootfs_mount(dev, fs_type);
}

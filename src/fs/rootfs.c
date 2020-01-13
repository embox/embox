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

#include <fs/mount.h> /* mount */
#include <fs/fs_driver.h>
#include <fs/vfs.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(unit_init);

#define DEVFS_DIR "/dev"
static int rootfs_mount(const char *dev, const char *fs_type) {
	struct path node, root;
	mode_t mode;

	if (-1 == mount((char *) dev, "/", (char *) fs_type)) {
		return -errno;
	}

	/* Handle devfs in a special way as automount is not implemented yet */
	mode = S_IFDIR | S_IRALL | S_IWALL | S_IXALL;
	vfs_get_root_path(&root);

	if (0 != vfs_create(&root, DEVFS_DIR, mode, &node)) {
		return -1;
	}

	if (-1 == mount(NULL, DEVFS_DIR, "devfs")) {
		return -errno;
	}

	return 0;
}

static int unit_init(void) {
	const char *dev, *fs_type;

	dev = OPTION_STRING_GET(bdev);
	fs_type = OPTION_STRING_GET(fstype);

	return rootfs_mount(dev, fs_type);
}

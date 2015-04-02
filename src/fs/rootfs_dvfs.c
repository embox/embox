/**
 * @file
 * @details
 *
 * @date 2 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <string.h>

#include <fs/dvfs.h>

#include <embox/unit.h>

EMBOX_UNIT_INIT(rootfs_mount);

struct super_block *rootfs_sb(void) {
	static struct super_block sb = {
		.fs_drv = NULL,
		.bdev   = NULL,
	};

	return &sb;
}

static int do_mount(const char *dev, const char *fs_type) {
	struct dumb_fs_driver *fsdrv;

	/* mount dev filesystem */
	fsdrv = dumb_fs_driver_find("devfs");
	if (fsdrv) {
		//fsdrv->fsop->mount("/dev", NULL);
	}

	if (-1 == dvfs_mount((char *) dev, "/", (char *) fs_type, 0)) {
		return -errno;
	}

	return 0;
}

static int rootfs_mount(void) {
	const char *dev, *fs_type;

	dev = OPTION_STRING_GET(bdev);
	fs_type = OPTION_STRING_GET(fstype);

	return do_mount(dev, fs_type);
}

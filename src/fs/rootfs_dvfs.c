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
#include <kernel/task/resource/file_table.h>

EMBOX_UNIT_INIT(rootfs_mount);

static struct super_block *root_sb;

extern int dvfs_update_root(void);

static int set_rootfs_sb(struct super_block *sb) {
	root_sb = sb;
	return 0;
}

struct super_block *rootfs_sb(void) {
	return root_sb;
}

static int do_mount(const char *dev, const char *fs_type) {
	assert(fs_type);
	struct dumb_fs_driver *fsdrv;

	fsdrv = dumb_fs_driver_find(fs_type);
	if (fsdrv == NULL || fsdrv->alloc_sb == NULL)
		return -ENOENT;

	set_rootfs_sb(fsdrv->alloc_sb((char*) dev));
	dvfs_update_root();

	if (dev) {
		/* TODO mount dev filesystem */
		fsdrv = dumb_fs_driver_find("devfs");
		if (fsdrv) {
			//fsdrv->fsop->mount("/dev", NULL);
		}
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

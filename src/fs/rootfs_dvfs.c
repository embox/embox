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
#include <embox/block_dev.h>
#include <embox/device.h>
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

/* @brief Perform mount of the root file system
 * @param dev     Path to the block device (e.g. /dev/sda1)
 * @param fs_type Name of the file system driver
 *
 * @return Negative error number
 * @retval       0 Ok
 * @revtal -ENOENT File system driver not found
 */
static int do_mount(const char *dev, const char *fs_type) {
	struct dumb_fs_driver *fsdrv;
	struct block_dev *bdev = NULL;

	assert(fs_type);

	fsdrv = dumb_fs_driver_find(fs_type);
	if (fsdrv == NULL)
		return -ENOENT;

	if (dev) {
		/* TODO mount dev filesystem */
		block_devs_init();
		bdev = block_dev_find(dev);
		if (bdev) {
			//fsdrv->fsop->mount("/dev", NULL);
		}
	}

	set_rootfs_sb(dvfs_alloc_sb(fsdrv, bdev));
	dvfs_update_root();

	if (-1 == dvfs_mount(bdev, "/", (char *) fs_type, 0)) {
		return -errno;
	}

	return 0;
}

/* @brief Starting initialization of the VFS
 */
static int rootfs_mount(void) {
	const char *dev, *fs_type;

	dev = OPTION_STRING_GET(bdev);
	fs_type = OPTION_STRING_GET(fstype);

	return do_mount(dev, fs_type);
}

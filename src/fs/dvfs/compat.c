/**
 * @file
 * @brief Handle calls from old VFS
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-06-10
 */

#include <assert.h>
#include <errno.h>

#include <drivers/block_dev.h>
#include <fs/dvfs.h>
#include <fs/fsop.h>

int mkfs(const char *blk_name, const char *fs_type, char *fs_spec) {
	const struct fs_driver *drv = fs_driver_find(fs_type);
	struct block_dev *bdev;
	struct lookup lu = {};
	int err;

	if (!drv) {
		return -EINVAL;
	}

	if ((err = dvfs_lookup(blk_name, &lu))) {
		return err;
	}

	if (!lu.item) {
		return -ENOENT;
	}

	assert(lu.item->d_inode);
	assert(lu.item->d_inode->i_privdata);

	bdev = dev_module_to_bdev(lu.item->d_inode->i_privdata);

	return drv->format(bdev, fs_spec);
}

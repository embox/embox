/*
 * @file
 *
 * @date Nov 29, 2012
 * @author: Anton Bondarev
 */

#include <errno.h>

#include <fs/fs_driver.h>
#include <fs/inode.h>
#include <fs/perm.h>

#include <fs/path.h>

#include <fs/kfsop.h>

int kformat(const char *pathname, const char *fs_type) {
	struct path node;
	const struct fs_driver *drv;
	int res;
	struct block_dev *bdev;

	if (0 != fs_type) {
		drv = fs_driver_find((const char *) fs_type);
		if (NULL == drv) {
			return -EINVAL;
		}
		if (NULL == drv->format) {
			return  -ENOSYS;
		}
	}
	else {
		return -EINVAL;
	}

	if (0 != (res = fs_perm_lookup(pathname, NULL, &node))) {
		errno = res == -ENOENT ? ENODEV : -res;
		return -1;
	}

	if (0 != (res = fs_perm_check(node.node, S_IWOTH))) {
		errno = EACCES;
		return -1;
	}

	bdev = node.node->i_privdata;
	if (0 != (res = drv->format(bdev, NULL))) {
		errno = -res;
		return -1;
	}

	return 0;
}


/**
 * @file
 *
 * @brief
 *
 * @date 29.05.2012
 * @author Andrey Gazukin
 */

#include <errno.h>
#include <fcntl.h>
#include <fs/kfsop.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/perm.h>
#include <fs/kfile.h>
#include <sys/types.h>

int mount(char *dev,  char *dir, char *fs_type) {
	int ret;

	ret = kmount(dev, dir, fs_type);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int format(const char *pathname, const char *fs_type) {
	int ret;

	ret = kformat(pathname, fs_type);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

int umount(char *dir) {
	int ret;

	ret = kumount(dir);
	if (ret != 0) {
		SET_ERRNO(-ret);
		return -1;
	}

	return 0;
}

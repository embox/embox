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
	return kmount(dev, dir, fs_type);
}

int format(const char *pathname, const char *fs_type) {
	return kformat(pathname, fs_type);
}

int umount(char *dir) {
	return kumount(dir);
}

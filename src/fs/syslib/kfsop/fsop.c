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
#include <fs/inode.h>
#include <fs/vfs.h>
#include <fs/perm.h>
#include <fs/kfile.h>
#include <sys/types.h>

int mount(const char *source, const char *target, const char *fs_type) {
	return kmount(source, target, fs_type);
}

int format(const char *pathname, const char *fs_type) {
	return kformat(pathname, fs_type);
}

int umount(char *dir) {
	return kumount(dir);
}

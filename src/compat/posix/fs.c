/**
 * @file
 *
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <errno.h>
#include <fcntl.h>
#include <fs/kfsop.h>
#include <fs/node.h>
#include <fs/vfs.h>
#include <fs/perm.h>
#include <fs/kfile.h>

int creat(const char *pathname, mode_t mode) {
	return open(pathname, O_CREAT | O_WRONLY | O_TRUNC, mode);
}

int mkdir(const char *pathname, mode_t mode) {
	return kmkdir(NULL, pathname, mode);
}

int remove(const char *pathname) {
	return kremove(pathname);
}

int unlink(const char *pathname) {
	return kunlink(pathname);
}

int rmdir(const char *pathname) {
	return krmdir(pathname);
}

int lstat(const char *path, struct stat *buf) {
	return klstat(path, buf);
}

int stat(const char *path, struct stat *buf) {
	return lstat(path, buf);
}

int truncate(const char *path, off_t length) {
	node_t *node;
	int res;

	if (0 == (res = fs_perm_lookup(vfs_get_root(), path, NULL, &node))) {
		errno = -res;
		return -1;
	}

	return ktruncate(node, length);
}

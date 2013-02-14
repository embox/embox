/**
 * @file
 *
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <types.h>
#include <fcntl.h>
#include <fs/kfsop.h>

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

/**
 * @file
 *
 * @brief
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <fcntl.h>

int creat(const char *pathname, mode_t mode) {
	int rc;
	rc = open(pathname, O_CREAT | O_WRONLY | O_TRUNC, mode);
	DPRINTF(("creat(%s, %d ...) = %d\n", pathname, mode, rc));
	return rc;
}

int mkdir(const char *pathname, mode_t mode) {
	return 0;
}

int remove(const char *pathname) {
	return 0;
}

int unlink(const char *pathname) {
	return 0;
}

int rmdir(const char *pathname) {
	return 0;
}

int lstat(const char *path, struct stat *buf) {
	return 0;
}

int stat(const char *path, struct stat *buf) {
	return 0;
}

int truncate(const char *path, off_t length) {
	return 0;
}

int flock(int fd, int operation) {
	return 0;
}


/**
 * @file
 *
 * @brief
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <fcntl.h>

#include <fs/dvfs.h>

int mkdir(const char *pathname, mode_t mode) {
	return creat(pathname, mode | O_DIRECTORY);
}

int remove(const char *pathname) {
	return dvfs_remove(pathname);
}

int unlink(const char *pathname) {
	return 0;
}

int rmdir(const char *pathname) {
	return 0;
}

int truncate(const char *path, off_t length) {
	return 0;
}

int flock(int fd, int operation) {
	return 0;
}


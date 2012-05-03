/**
 * @file
 *
 * @brief
 *
 * @date 16.09.2011
 * @author Anton Bondarev
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include <fs/fs.h>
#include <fcntl.h>

int remove(const char *pathname) {
	return 0;
}

int unlink(const char *pathname) {
	return 0;
}

int rmdir(const char *pathname) {
	return 0;
}

int mkdir(const char *path, mode_t __mode) {
	return 0;
}

int creat(const char *pathname, mode_t mode) {
	return 0;
}

int fstat(const char *path, stat_t *buf) {
	return 0;
}


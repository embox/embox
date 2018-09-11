/**
 * @file
 *
 * @data 12 oct. 2015
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <fs/dvfs.h>

int stat(const char *path, struct stat *buf) {
	int fd, ret;

	assert(path);
	assert(buf);

	fd = open(path, O_RDONLY); /* Actually, flag should be smth O_PATH */

	ret = fstat(fd, buf);

	close(fd);

	return ret;
}

int lstat(const char *path, struct stat *buf) {
	return stat(path, buf);
}

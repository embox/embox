/**
 * @brief
 *
 * @date 12.02.24
 * @author Aleksey Zhmulin
 */

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int stat(const char *path, struct stat *buf) {
	int ret;
	int fd;

	fd = open(path, O_PATH);
	if (fd < 0) {
		return -1;
	}

	ret = fstat(fd, buf);

	close(fd);

	return ret;
}

int lstat(const char *path, struct stat *buf) {
	return stat(path, buf);
}

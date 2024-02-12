/**
 * @file
 *
 * @brief
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

int truncate(const char *path, off_t length) {
	int fd, ret;

	if (!path) {
		return SET_ERRNO(-EINVAL);
	}

	fd = open(path, O_WRONLY);
	if (fd < 0) {
		return fd;
	}

	ret = ftruncate(fd, length);

	close(fd);

	return ret;
}

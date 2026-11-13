/**
 * @file
 *
 * @date 12 oct. 2015
 * @author: Anton Bondarev
 */
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

int stat(const char *path, struct stat *buf) {
	int fd, ret;

	assert(path);
	assert(buf);

	fd = open(path, O_RDONLY | O_PATH); /* Actually, flag should be smth O_PATH */

	if (fd == -1 && errno == EISDIR) {
		/* Stub */
		*buf = (struct stat) {
			.st_mode = S_IFDIR | S_IRWXA,
		};
		return 0;
	}

	if (fd == -1) {
		/* Keep the errno open() set; fstat(-1) would replace it with EBADF. */
		return -1;
	}

	ret = fstat(fd, buf);

	close(fd);

	return ret;
}

__strong_alias(lstat, stat);

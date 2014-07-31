/**
 * @file
 *
 * @date Jun 6, 2014
 * @author: Anton Bondarev
 */

#include <errno.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>

size_t fwrite(const void *buf, size_t size, size_t count, FILE *file) {
	int ret;

	if (NULL == file) {
		return -1;
	}

	if (funopen_check(file)) {
		if (file->writefn) {
			ret = file->writefn((void *) file->cookie, buf, size * count);
		} else {
			ret = 0;
		}
	} else {
		ret = write(file->fd, buf, size * count);
	}

	return ret > 0 ? ret / size : 0;
}

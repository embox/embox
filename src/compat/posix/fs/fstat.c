/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */
#include <sys/stat.h>
#include <errno.h>

#include <fs/index_descriptor.h>


int fstat(int fd, struct stat *buff) {
	int res;

	if (!buff) {
		return -EINVAL;
	}
	res = index_descriptor_fstat(fd, buff);
	if (res < 0) {
		return SET_ERRNO(-res);
	}
	return 0;
}

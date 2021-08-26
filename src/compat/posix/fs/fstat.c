/**
 * @file
 *
 * @date 15.11.13
 * @author Anton Bondarev
 */

#include <sys/stat.h>
#include <errno.h>
#include <stddef.h>
#include <assert.h>

#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc_table.h>

int fstat(int fd, struct stat *buff) {
	int ret;

	assert(buff != NULL);

	if (!idesc_index_valid(fd)
			|| (NULL == index_descriptor_get(fd))) {
		return SET_ERRNO(EBADF);
	}

	ret = index_descriptor_fstat(fd, buff);
	if (ret != 0) {
		return SET_ERRNO(-ret);
	}

	return 0;
}

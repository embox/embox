/**
 * @file
 *
 * @date 15.11.13
 * @author Anton Bondarev
 */

#include <errno.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc_table.h>
#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc.h>

int close(int fd) {
	int ret;
	struct idesc *idesc;

	if (!idesc_index_valid(fd)
			|| (NULL == (idesc = index_descriptor_get(fd)))) {
		return SET_ERRNO(EBADF);
	}

	ret = idesc_close(idesc, fd);
	if (ret != 0) {
		return SET_ERRNO(-ret);
	}

	return 0;
}

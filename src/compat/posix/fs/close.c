/**
 * @file
 *
 * @date Nov 15, 2013
 * @author: Anton Bondarev
 */
#include <errno.h>

#include <kernel/task.h>
#include <kernel/task/idesc_table.h>
#include <fs/index_descriptor.h>
#include <fs/idesc.h>

int close(int fd) {
	struct idesc *idesc;
	int res;

	if(!idesc_index_valid(fd)) {
		return SET_ERRNO(EBADF);
	}

	idesc = index_descriptor_get(fd);
	if(!idesc) {
		return SET_ERRNO(EBADF);
	}

	res = idesc_close(idesc, fd);
	if (res) {
		return SET_ERRNO(-res);
	}

	return res;
}

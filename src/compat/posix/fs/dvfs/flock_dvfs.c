/**
 * @file
 * @brief
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <sys/file.h>

#include <fs/dvfs.h>
#include <fs/file_desc.h>
#include <kernel/task.h>
#include <kernel/task/resource/idesc.h>
#include <kernel/task/resource/idesc_table.h>
#include <kernel/task/resource/index_descriptor.h>

int flock(int fd, int operation) {
	extern const struct idesc_ops idesc_file_ops;
	struct idesc *idesc;
	int res;

	if (!idesc_index_valid(fd)
	    || (NULL == (idesc = index_descriptor_get(fd)))) {
		return SET_ERRNO(EBADF);
	}

	if (idesc->idesc_ops != &idesc_file_ops) {
		/* A device or a socket has nothing to lock */
		return SET_ERRNO(EINVAL);
	}

	res = dvfs_flock(file_desc_from_idesc(idesc), operation);
	if (res) {
		return SET_ERRNO(-res);
	}

	return 0;
}

/**
 * @file
 *
 * @date 15.11.13
 * @author Anton Bondarev
 */

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include <fs/kfile.h>
#include <fs/file_desc.h>
#include <fs/index_descriptor.h>
#include <fs/idesc.h>

#include <kernel/task/idesc_table.h>

off_t lseek(int fd, off_t offset, int origin) {
	struct file_desc *desc;
	off_t ret;

	if (!idesc_index_valid(fd)
			|| (NULL == index_descriptor_get(fd))) {
		return SET_ERRNO(EBADF);
	}

	desc = file_desc_get(fd);
	if (desc == NULL) {
		return -ESPIPE;
	}

	ret = kseek(desc, offset, origin);
	if (ret < 0) {
		return SET_ERRNO(-ret);
	}

	return ret;
}

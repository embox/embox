/**
 * @file
 *
 * @date 3 Apr 2015
 * @author Denis Deryugin
 */

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fs/dvfs.h>
#include <fs/index_descriptor.h>
#include <kernel/task/resource/idesc_table.h>

off_t lseek(int fd, off_t offset, int origin) {
	struct stat sb;
	struct file *file;

	if (!idesc_index_valid(fd)) {
		return SET_ERRNO(EBADF);
	}
	if (NULL == (file = (struct file *)index_descriptor_get(fd))) {
		return SET_ERRNO(EBADF);
	}

	fstat(fd, &sb);
	switch (sb.st_mode & S_IFMT) {
		case S_IFIFO:
		case S_IFSOCK:
		return SET_ERRNO(EPIPE);
	}

	switch (origin) {
		case SEEK_SET:
			file->pos = offset;
			break;

		case SEEK_CUR:
			file->pos += offset;
			break;

		case SEEK_END:
			file->pos = sb.st_size + offset;
			break;

		default:
			return -EINVAL;
	}
	return file->pos;
}

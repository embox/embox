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

#include <fs/file_desc.h>
#include <kernel/task/resource/index_descriptor.h>
#include <kernel/task/resource/idesc_table.h>

off_t lseek(int fd, off_t offset, int origin) {
	struct stat sb;
	struct file_desc *file;
	off_t pos;

	if (!idesc_index_valid(fd)) {
		return SET_ERRNO(EBADF);
	}
	if (NULL == (file = (struct file_desc *)index_descriptor_get(fd))) {
		return SET_ERRNO(EBADF);
	}

	fstat(fd, &sb);
	switch (sb.st_mode & S_IFMT) {
		case S_IFIFO:
		case S_IFSOCK:
		return SET_ERRNO(ESPIPE);
	}

	pos = file_get_pos(file);
	switch (origin) {
		case SEEK_SET:
			pos = 0;
			break;

		case SEEK_CUR:
			break;

		case SEEK_END:
			pos = sb.st_size;
			break;

		default:
			return SET_ERRNO(EINVAL);
	}
	if (__builtin_add_overflow(pos, offset, &pos)) {
		return SET_ERRNO(EOVERFLOW);
	}
	/* A position before the start is refused, and the old one kept. */
	if (pos < 0) {
		return SET_ERRNO(EINVAL);
	}
	file_set_pos(file, pos);
	return pos;
}

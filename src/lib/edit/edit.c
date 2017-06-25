/**
 * @file
 * @brief Tools for file edition
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2016-01-15
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <lib/edit.h>
#include <../../../compat/libc/stdio/file_struct.h>

int fdelete_from_file(int fd, size_t length) {
	char buf[128];
	int i;
	int err;
	off_t size;
	off_t pos;
	int cnt;

	memset(buf, 0, sizeof(buf));

	pos = lseek(fd, 0, SEEK_CUR);
	size = lseek(fd, 0, SEEK_END);

	cnt = (size - (pos + length) + sizeof(buf) - 1) / sizeof(buf);

	for (i = 0; i < cnt; i++) {
		if (lseek(fd, pos + sizeof(buf) * i + length, SEEK_SET) < 0)
			return -1;
		if (read(fd, buf, sizeof(buf)) < 0)
			return -1;
		if (lseek(fd, pos + sizeof(buf) * i, SEEK_SET) < 0)
			return -1;
		if (write(fd, buf, sizeof(buf)))
			return -1;
	}

	if ((err = ftruncate(fd, size - length)))
		return err;

	return 0;
}

int finsert_into_file(int fd, char *buf, size_t length) {
	char tmp[128];
	int i;
	int err;
	off_t size;
	off_t pos;
	int cnt;
	assert(buf);

	memset(tmp, 0, sizeof(tmp));

	pos = lseek(fd, 0, SEEK_CUR);
	size = lseek(fd, 0, SEEK_END);

	cnt = (size - pos + sizeof(tmp) - 1) / sizeof(tmp);

	for (i = cnt - 1; i >= 0; i--) {
		if (lseek(fd, pos + sizeof(tmp) * i, SEEK_SET) < 0)
			return -1;
		if (read(fd, tmp, sizeof(tmp)) < 0)
			return -1;
		if (lseek(fd, pos + sizeof(tmp) * i + length, SEEK_SET) < 0)
			return -1;
		if (write(fd, tmp, sizeof(tmp)) < 0)
			return -1;
	}

	if (lseek(fd, pos, SEEK_SET) < 0)
		return -1;

	if (write(fd, buf, length) < 0)
		return -1;

	if ((err = ftruncate(fd, size + length)))
		return err;

	return 0;
}

int delete_from_file(FILE *file, size_t length) {
	return fdelete_from_file(file->fd, length);
}

int insert_into_file(FILE *file, char *buf, size_t length) {
	return finsert_into_file(file->fd, buf, length);
}

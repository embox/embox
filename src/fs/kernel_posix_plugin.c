/**
 * @file
 * @brief File descriptor (fd) abstraction over FILE *
 * @details Provide POSIX kernel support to operate with flides (int)
 *	instead of FILE *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#include <kernel/task.h>
#include <kernel/task/idx.h>
#include <util/array.h>
#include <stdio.h>
#include <fcntl.h>

#include <fs/posix.h>

static int this_close(struct idx_desc_data *data) {
	FILE *file = (FILE *) data;
	return fclose(file);
}

static ssize_t this_read(struct idx_desc_data *data, void *buf, size_t nbyte) {
	FILE *file = (FILE *) data;
	return fread(buf, 1, nbyte, file);
}

static ssize_t this_write(struct idx_desc_data *data, const void *buf, size_t nbyte) {
	FILE *file = (FILE *) data;
	return fwrite(buf, 1, nbyte, file);
}

static int this_lseek(struct idx_desc_data *data, long int offset, int origin) {
	FILE *file = (FILE *) data;
	return fseek(file, offset, origin);
}

static int this_ioctl(struct idx_desc_data *data, int request, va_list args) {
	FILE *file = (FILE *) data;
	return fioctl(file, request, args);
}

const struct task_idx_ops task_idx_ops_file = {
	.close = this_close,
	.read  = this_read,
	.write = this_write,
	.fseek = this_lseek,
	.ioctl = this_ioctl,
};

/**
 * @file
 * @brief File descriptor (fd) abstraction over FILE *
 * @details Provide POSIX kernel support to operate with flides (int)
 *	instead of FILE *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#include <kernel/task.h>
#include <util/array.h>
#include <stdio.h>
#include <fcntl.h>

#include <fs/posix.h>

static int this_close(void *data) {
	FILE *file = (FILE *) data;
	return fclose(file);
}

static ssize_t this_read(void *data, void *buf, size_t nbyte) {
	FILE *file = (FILE *) data;
	return fread(buf, 1, nbyte, file);
}

static ssize_t this_write(void *data, const void *buf, size_t nbyte) {
	FILE *file = (FILE *) data;
	return fwrite(buf, 1, nbyte, file);
}

static int this_lseek(void *data, long int offset, int origin) {
	FILE *file = (FILE *) data;
	return fseek(file, offset, origin);
}

static int this_ioctl(void *data, int request, va_list args) {
	FILE *file = (FILE *) data;
	return fioctl(file, request, args);
}

struct task_res_ops task_res_ops_file = {
	.close = this_close,
	.read  = this_read,
	.write = this_write,
	.fseek = this_lseek,
	.ioctl = this_ioctl,
};

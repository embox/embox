/**
 * @file
 * @brief File descriptor (fd) abstraction over FILE *
 * @details Provide POSIX kernel support to operate with flides (int)
 *	instead of FILE *
 * @date 06.09.11
 * @author Anton Kozlov
 */

#include <assert.h>
#include <errno.h>
#include <kernel/task.h>
#include <kernel/task/idx.h>

#include <fs/flags.h>
#include <fs/kfile.h>


extern const struct idesc_ops task_idx_ops_file;
static int this_close(struct idesc *idesc) {
	assert(idesc);
	assert(idesc->idesc_ops == &task_idx_ops_file);
	return kclose((struct file_desc *)idesc);
}

static ssize_t this_read(struct idesc *idesc, void *buf, size_t nbyte) {
	assert(idesc);
	assert(idesc->idesc_ops == &task_idx_ops_file);
	return kread(buf, nbyte, (struct file_desc *)idesc);
}

static ssize_t this_write(struct idesc *idesc, const void *buf, size_t nbyte) {
	assert(idesc);
	assert(idesc->idesc_ops == &task_idx_ops_file);
	return kwrite(buf, nbyte, (struct file_desc *)idesc);
}


static int this_stat(struct idesc *idesc, void *buf) {
	assert(idesc);
	assert(idesc->idesc_ops == &task_idx_ops_file);
	return kfstat((struct file_desc *)idesc, buf);
}

static int this_ioctl(struct idesc *idesc, int request, void *data) {
	assert(idesc);
	assert(idesc->idesc_ops == &task_idx_ops_file);
	return kioctl((struct file_desc *)idesc, request, data);
}


const struct idesc_ops task_idx_ops_file = {
	.close = this_close,
	.read  = this_read,
	.write = this_write,
	.ioctl = this_ioctl,
	.fstat = this_stat,
};


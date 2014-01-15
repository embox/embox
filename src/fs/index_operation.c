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

#include <fs/flags.h>
#include <fs/kfile.h>


extern const struct idesc_ops idesc_file_ops;

static void idesc_file_ops_close(struct idesc *idesc) {
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_file_ops);
	kclose((struct file_desc *)idesc);
}

static ssize_t idesc_file_ops_read(struct idesc *idesc, void *buf, size_t nbyte) {
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_file_ops);
	return kread(buf, nbyte, (struct file_desc *)idesc);
}

static ssize_t idesc_file_ops_write(struct idesc *idesc, const void *buf, size_t nbyte) {
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_file_ops);
	return kwrite(buf, nbyte, (struct file_desc *)idesc);
}


static int idesc_file_ops_stat(struct idesc *idesc, void *buf) {
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_file_ops);
	return kfstat((struct file_desc *)idesc, buf);
}

static int idesc_file_ops_ioctl(struct idesc *idesc, int request, void *data) {
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_file_ops);
	return kioctl((struct file_desc *)idesc, request, data);
}

static int idesc_file_ops_status(struct idesc *idesc, int mask) {
	assert(idesc);
	assert(idesc->idesc_ops == &idesc_file_ops);
	return 1;
}

const struct idesc_ops idesc_file_ops = {
	.close = idesc_file_ops_close,
	.read  = idesc_file_ops_read,
	.write = idesc_file_ops_write,
	.ioctl = idesc_file_ops_ioctl,
	.fstat = idesc_file_ops_stat,
	.status = idesc_file_ops_status,
};


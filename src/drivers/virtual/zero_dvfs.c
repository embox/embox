/**
 * @file
 * @brief Handle /dev/zero. Ported from old vfs, originally by Anton Kozlov
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <util/err.h>

#include <drivers/char_dev.h>
#include <fs/dvfs.h>

#define ZERO_DEV_NAME "zero"

/* forward declaration */
static struct file_operations zero_ops;
static struct idesc_ops idesc_cdev_zero_ops;

static struct idesc *zero_open(struct inode *node, struct idesc *idesc) {
	struct file *file;
	file = dvfs_alloc_file();
	if (!file) {
		return err_ptr(ENOMEM);
	}
	*file = (struct file) {
		.f_idesc  = {
				.idesc_ops   = &idesc_cdev_zero_ops,
		},
	};
	return &file->f_idesc;
}

static void zero_close(struct idesc *desc) {
	dvfs_destroy_file((struct file *)desc);
}

static ssize_t zero_read(struct idesc *desc, const struct iovec *iov, int cnt) {
	void *buf;
	size_t nbyte;

	assert(iov);
	buf = iov->iov_base;
	assert(cnt == 1);
	nbyte = iov->iov_len;

	memset(buf, 0, nbyte);

	return nbyte;
}

static ssize_t zero_write(struct idesc *desc, const void *buf, size_t size) {
	return size;
}

static struct file_operations zero_ops = {
		.open = zero_open,
};

static struct idesc_ops idesc_cdev_zero_ops = {
	.close = zero_close,
	.id_readv  = zero_read,
	.write = zero_write,
};

CHAR_DEV_DEF(ZERO_DEV_NAME, &zero_ops, &idesc_cdev_zero_ops, NULL);

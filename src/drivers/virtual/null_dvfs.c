/**
 * @file
 * @brief Creates /dev/null
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version 0.1
 * @date 2015-10-05
 */
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <util/err.h>

#include <drivers/char_dev.h>
#include <fs/dvfs.h>

#define NULL_DEV_NAME "null"

static void null_close(struct idesc *desc) {
}

static ssize_t null_write(struct idesc *desc, const struct iovec *iov, int cnt) {
	int i;
	ssize_t ret_size;

	ret_size = 0;
	for (i = 0; i < cnt; i++) {
		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static ssize_t null_read(struct idesc *desc, const struct iovec *iov, int cnt) {
	return 0;
}

static struct idesc_ops idesc_cdev_null_ops = {
	.close = null_close,
	.id_readv = null_read,
	.id_writev = null_write,
	.fstat     = char_dev_idesc_fstat,
};

CHAR_DEV_DEF(NULL_DEV_NAME, NULL, &idesc_cdev_null_ops, NULL);

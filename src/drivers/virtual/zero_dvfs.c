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

static void zero_close(struct idesc *desc) {
}

static ssize_t zero_read(struct idesc *desc, const struct iovec *iov, int cnt) {
	int i;
	ssize_t ret_size;

	assert(iov);

	ret_size = 0;
	for (i = 0; i < cnt; i++) {
		memset(iov[i].iov_base, 0, iov[i].iov_len);

		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static ssize_t zero_write(struct idesc *desc, const struct iovec *iov, int cnt) {
	int i;
	ssize_t ret_size;

	ret_size = 0;
	for (i = 0; i < cnt; i++) {
		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static struct idesc_ops idesc_cdev_zero_ops = {
	.close     = zero_close,
	.id_readv  = zero_read,
	.id_writev = zero_write,
	.fstat     = char_dev_idesc_fstat,
};

CHAR_DEV_DEF(ZERO_DEV_NAME, NULL, &idesc_cdev_zero_ops, NULL);

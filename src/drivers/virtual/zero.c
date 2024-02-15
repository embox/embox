/**
 * @file
 * @brief Creates file /dev/zero
 *
 * @date 08.09.11
 * @author Anton Kozlov -- original version
 * @author Denis Deryugin <deryugin.denis@gmail.com> -- port to new vfs
 */
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>

#include <drivers/char_dev.h>
#include <util/err.h>
#include <util/macro.h>

#define ZERO_DEV_NAME zero

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

static ssize_t zero_write(struct idesc *desc, const struct iovec *iov,
    int cnt) {
	int i;
	ssize_t ret_size;

	ret_size = 0;
	for (i = 0; i < cnt; i++) {
		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static const struct idesc_ops zero_idesc_ops = {
    .open = char_dev_default_open,
    .close = char_dev_default_close,
    .fstat = char_dev_default_fstat,
    .id_readv = zero_read,
    .id_writev = zero_write,
};

CHAR_DEV_DEF(ZERO_DEV_NAME, &zero_idesc_ops, NULL);

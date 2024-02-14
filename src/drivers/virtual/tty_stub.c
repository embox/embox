/**
 * @file
 * @brief
 *
 * @author Aleksey Zhmulin
 * @date 20.05.23
 */
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>

#include <drivers/char_dev.h>
#include <framework/mod/options.h>
#include <util/err.h>

#define TTY_NAME OPTION_MODULE_GET(__EMBUILD_MOD__, STRING, tty_name)

static ssize_t tty_stub_write(struct idesc *desc, const struct iovec *iov,
    int cnt) {
	int i;
	ssize_t ret_size;

	ret_size = 0;
	for (i = 0; i < cnt; i++) {
		ret_size += iov[i].iov_len;
	}

	return ret_size;
}

static ssize_t tty_stub_read(struct idesc *desc, const struct iovec *iov,
    int cnt) {
	sleep(-1);
	return 0;
}

static int tty_stub_ioctl(struct idesc *idesc, int request, void *data) {
	return 0;
}

static const struct idesc_ops tty_stub_idesc_ops = {
    .open = char_dev_default_open,
    .close = char_dev_default_close,
    .fstat = char_dev_default_fstat,
    .id_readv = tty_stub_read,
    .id_writev = tty_stub_write,
    .ioctl = tty_stub_ioctl,
};

CHAR_DEV_DEF(TTY_NAME, &tty_stub_idesc_ops, NULL);

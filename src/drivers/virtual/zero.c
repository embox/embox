/**
 * @file
 * @brief Creates file /dev/zero
 *
 * @date 08.09.11
 * @author Anton Kozlov -- original version
 * @author Denis Deryugin <deryugin.denis@gmail.com> -- port to new vfs
 */

#include <stddef.h>
#include <string.h>

#include <drivers/char_dev.h>

static ssize_t zero_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	memset(buf, 0, nbyte);
	return nbyte;
}

static ssize_t zero_write(struct char_dev *cdev, const void *buf,
    size_t nbyte) {
	return nbyte;
}

static const struct char_dev_ops zero_cdev_ops = {
    .read = zero_read,
    .write = zero_write,
};

static struct char_dev zero_cdev = CHAR_DEV_INIT(zero_cdev, "zero",
    &zero_cdev_ops);

CHAR_DEV_REGISTER(&zero_cdev);

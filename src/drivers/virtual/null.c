/**
 * @file
 * @brief Creates /dev/null
 *
 * @date 08.09.11
 * @author Anton Kozlov -- original implementation
 * @author Denis Deryugin <deryugin.denis@gmail.com> -- port to new vfs
 */

#include <stddef.h>

#include <drivers/char_dev.h>

static ssize_t null_read(struct char_dev *cdev, void *buf, size_t nbyte) {
	return 0;
}

static ssize_t null_write(struct char_dev *cdev, const void *buf,
    size_t nbyte) {
	return nbyte;
}

static const struct char_dev_ops null_cdev_ops = {
    .read = null_read,
    .write = null_write,
};

static struct char_dev null_cdev = CHAR_DEV_INIT(null_cdev, "null",
    &null_cdev_ops);

CHAR_DEV_REGISTER(&null_cdev);

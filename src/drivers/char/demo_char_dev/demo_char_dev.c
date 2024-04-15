/**
 * @file
 *
 * @date Mar 28, 2022
 * @author Anton Bondarev
 */

#include <stddef.h>
#include <sys/types.h>

#include <drivers/char_dev.h>

static ssize_t demo_char_dev_read(struct char_dev *cdev, void *buf,
    size_t nbyte) {
	return 0;
}

static ssize_t demo_char_dev_write(struct char_dev *cdev, const void *buf,
    size_t nbyte) {
	return 0;
}

static void *demo_char_dev_ioctl(struct char_dev *cdev, int request,
    void *data) {
	return 0;
}

static const struct char_dev_ops demo_char_dev_ops = {
    .read = demo_char_dev_read,
    .write = demo_char_dev_write,
    .ioctl = demo_char_dev_ioctl,
};

static struct char_dev demo_char_dev = CHAR_DEV_INIT(demo_char_dev,
    "demo_char_dev", &demo_char_dev_ops);

CHAR_DEV_REGISTER(&demo_char_dev);

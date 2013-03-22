/**
 * @file
 * @brief Creates file /dev/ttyX
 *
 * @date 04.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <limits.h>

#include <drivers/diag.h>
#include <drivers/video/fb.h>
#include <drivers/video/font.h>
#include <drivers/tty.h>

#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/node.h>

#include <embox/device.h>

#define TTY_DEV_NAME "tty0"
#define DEFAULT_FRAMEBUFFER "fb0"

/* forward declaration */
static int tty_device_init(void);
static const struct kfile_operations tty_device_ops;

EMBOX_DEVICE(TTY_DEV_NAME, &tty_device_ops, tty_device_init);

static int tty_device_open(struct node *node, struct file_desc *file_desc, int flags) {
	struct fb_info *info;

	info = fb_lookup(DEFAULT_FRAMEBUFFER);
	assert(info != NULL);

	file_desc->file_info = (void *)info;

	return 0;
}

static int tty_device_close(struct file_desc *desc) {
	return 0;
}

static size_t tty_device_read(struct file_desc *desc, void *buf, size_t size) {
	if (size == 0) return 0;

	*(char *)buf = iodev_getc();
	return sizeof(char);
}

static size_t tty_device_write(struct file_desc *desc, void *buf, size_t size) {
	size_t i;
	const char *data;

	data = (const char *)buf;
	for (i = 0; i < size; ++i)
		iodev_putc(data[i]);

	return size;
}

static int tty_device_ioctl(struct file_desc *desc, int request, va_list args) {
	switch(request) {
	case TTY_IOCTL_GETATTR:
		break;
	case TTY_IOCTL_SETATTR:
		break;
	default:
		return -ENOSYS;
	}
	return ENOERR;
}

static const struct kfile_operations tty_device_ops = {
	.open = tty_device_open,
	.close = tty_device_close,
	.read = tty_device_read,
	.write = tty_device_write,
	.ioctl = tty_device_ioctl
};

static int tty_device_init(void) {
	return char_dev_register(TTY_DEV_NAME, &tty_device_ops);
}

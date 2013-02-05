/**
 * @file
 * @brief Creates file /dev/ttyX
 *
 * @date 04.02.13
 * @author Ilia Vaprol
 */

#include <assert.h>
#include <drivers/diag.h>
#include <drivers/video/fb.h>
#include <drivers/video/font.h>
#include <errno.h>
#include <fs/file_desc.h>
#include <fs/file_operation.h>
#include <fs/node.h>
#include <stddef.h>
#include <string.h>
#include <embox/device.h>
#include <limits.h>

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

	*(char *)buf = diag_getc();
	return sizeof(char);
}

static size_t tty_device_write(struct file_desc *desc, void *buf, size_t size) {
	static const struct font_desc *font = &font_vga_8x8;
	static struct fb_image symbol = {
		.dx = 0,
		.dy = 0,
		.fg_color = 0xF0F0,
		.bg_color = 0x0000
	};
	size_t i;
	struct fb_info *info;
	const unsigned char *data;

	info = (struct fb_info *)desc->file_info;
	assert(info != NULL);

	symbol.width = font->width;
	symbol.height = font->height;

	data = (const unsigned char *)buf;
	for (i = 0; i < size; ++i) {
		symbol.data = font->data + data[i] * font->height * font->width / CHAR_BIT;
		info->ops->fb_imageblit(info, &symbol);
		symbol.dx += symbol.width;
	}

	return size;
}

static const struct kfile_operations tty_device_ops = {
	.open = tty_device_open,
	.close = tty_device_close,
	.read = tty_device_read,
	.write = tty_device_write,
};

static int tty_device_init(void) {
	return char_dev_register(TTY_DEV_NAME, &tty_device_ops);
}

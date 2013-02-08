/**
 * @file
 * @brief
 *
 * @date 07.02.13
 * @author Ilia Vaprol
 */

#include <drivers/iodev.h>
#include <drivers/keyboard.h>
#include <drivers/video/fb.h>
#include <drivers/video/font.h>

static int iodev_video_init(void) {
	return 0;
}

static char iodev_video_getc(void) {
	return keyboard_getc();
}

#include <assert.h>
static void iodev_video_putc(char ch) {
	static const struct font_desc *font = &font_vga_8x8;
	static struct fb_image symbol = {
		.dx = 0,
		.dy = 0,
		.fg_color = 0xF0F0,
		.bg_color = 0x0000,
		.depth = 1
	};
	struct fb_info *info;

	info = fb_lookup("fb0");
	assert(info != NULL);

	symbol.width = font->width;
	symbol.height = font->height;

	if (ch == '\n') {
		symbol.dx = 0;
		symbol.dy += symbol.height + 3;
	}
	else if (ch == '\r') {
		symbol.dx = 0;
	}
	else {
		if (symbol.dx + symbol.width > info->var.xres) {
			symbol.dx = 0;
			symbol.dy += symbol.height + 3;
		}
		symbol.data = font->data + ch * font->height * font->width / 8;
		info->ops->fb_imageblit(info, &symbol);
		symbol.dx += symbol.width;
	}
}

static int iodev_video_kbhit(void) {
	return keyboard_has_symbol();
}

static const struct iodev_ops iodev_video_ops_struct = {
	.init = &iodev_video_init,
	.getc = &iodev_video_getc,
	.putc = &iodev_video_putc,
	.kbhit = &iodev_video_kbhit
};

const struct iodev_ops *iodev_video_ops = &iodev_video_ops_struct;

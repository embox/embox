/**
 * @file
 * @brief
 *
 * @date 07.02.13
 * @author Ilia Vaprol
 */

#include <drivers/iodev.h>
#include <drivers/video/fb.h>
#include <drivers/video/font.h>
#include <drivers/tty.h>
#include <assert.h>
#include <stddef.h>
#include <drivers/diag.h>

struct video_tty_data {
	const struct font_desc *font;
	struct fb_info *fb;
	int fg_color;
	int bg_color;
	int cur_color;
};

static void video_tty_init(struct tty *t) { }

static void video_tty_cursor(struct tty *t, uint32_t x, uint32_t y) {
	struct fb_cursor cursor;
	struct video_tty_data *data;

	data = (struct video_tty_data *)t->data;
	assert(data != NULL);

	assert(data->font != NULL);

	cursor.enable = 1;
	cursor.rop = ROP_XOR;
	cursor.image.width = data->font->width;
	cursor.image.height = data->font->height;
	cursor.image.fg_color = data->cur_color;

	cursor.hot.x = x;
	cursor.hot.y = y;

	assert(data->fb != NULL);
	assert(data->fb->ops != NULL);
	assert(data->fb->ops->fb_cursor != NULL);
	data->fb->ops->fb_cursor(data->fb, &cursor);
}

static void video_tty_putc(struct tty *t, char ch, uint32_t x, uint32_t y) {
	struct fb_image symbol;
	struct video_tty_data *data;

	data = (struct video_tty_data *)t->data;
	assert(data != NULL);

	assert(data->font != NULL);

	symbol.dx = x * data->font->width;
	symbol.dy = y * data->font->height;
	symbol.width = data->font->width;
	symbol.height = data->font->height;
	symbol.fg_color = data->fg_color;
	symbol.bg_color = data->bg_color;
	symbol.depth = 1;
	symbol.data = data->font->data + (unsigned char)ch * data->font->height * data->font->width / 8;

	assert(data->fb != NULL);
	assert(data->fb->ops != NULL);
	assert(data->fb->ops->fb_imageblit != NULL);
	data->fb->ops->fb_imageblit(data->fb, &symbol);
}

static void video_tty_clear(struct tty *t, uint32_t x, uint32_t y,
		uint32_t width, uint32_t height) {
	struct fb_fillrect rect;
	struct video_tty_data *data;

	data = (struct video_tty_data *)t->data;
	assert(data != NULL);

	assert(data->font != NULL);

	rect.dx = x * data->font->width;
	rect.dy = y * data->font->height;
	rect.width = width * data->font->width;
	rect.height = height * data->font->height;
	rect.color = data->bg_color;
	rect.rop = ROP_COPY;

	assert(data->fb != NULL);
	assert(data->fb->ops != NULL);
	assert(data->fb->ops->fb_fillrect != NULL);
	data->fb->ops->fb_fillrect(data->fb, &rect);
}

static void video_tty_move(struct tty *t, uint32_t sx, uint32_t sy,
		uint32_t width, uint32_t height, uint32_t dx, uint32_t dy) {
	struct fb_copyarea area;
	struct video_tty_data *data;

	data = (struct video_tty_data *)t->data;
	assert(data != NULL);

	assert(data->font != NULL);

	area.dx = dx * data->font->width;
	area.dy = dy * data->font->height;
	area.width = width * data->font->width;
	area.height = height * data->font->height;
	area.sx = sx * data->font->width;
	area.sy = sy * data->font->height;

	assert(data->fb != NULL);
	assert(data->fb->ops != NULL);
	assert(data->fb->ops->fb_copyarea != NULL);
	data->fb->ops->fb_copyarea(data->fb, &area);
}

static struct tty video_tty;
static const struct tty_ops video_tty_ops = {
	.init = &video_tty_init,
	.cursor = &video_tty_cursor,
	.putc = &video_tty_putc,
	.clear = &video_tty_clear,
	.move = &video_tty_move
};

static int iodev_video_init(void) {
	static struct video_tty_data data;

	data.font = &font_vga_8x16;
	data.fb = fb_lookup("fb0");
	assert(data.fb != NULL);
	data.fg_color = 0x00F0;
	data.bg_color = 0xFFFF;
	data.cur_color = 0x00F0;

	tty_init(&video_tty, 80, 24, &video_tty_ops, &data);
	return 0;
}

static void iodev_video_putc(char ch) {
	tty_putc(&video_tty, ch);
}

static char iodev_video_getc(void) {
	return diag_getc();
}

static int iodev_video_kbhit(void) {
	return diag_kbhit();
}

static const struct iodev_ops iodev_video_ops_struct = {
	.init = &iodev_video_init,
	.getc = &iodev_video_getc,
	.putc = &iodev_video_putc,
	.kbhit = &iodev_video_kbhit
};

const struct iodev_ops *const iodev_video_ops = &iodev_video_ops_struct;

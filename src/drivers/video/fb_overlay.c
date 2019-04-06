/**
 * @file text_overlay.c
 * @brief Some functions to put text to frame
 * @author Denis Deryugin <deryugin.denis@gmail.com>
 * @version
 * @date 14.12.2017
 */

#include <string.h>

#include <drivers/video/fb.h>
#include <drivers/video/font.h>

static struct fb_info *text_fb = 0;
static void *overlay_sw_base = 0;
struct font_desc const *font = &font_vga_8x16;

#define CHAR_WIDTH  8
#define CHAR_HEIGHT 16

void fb_overlay_init(struct fb_info *fbi, void *base) {
	text_fb = fbi;
	overlay_sw_base = base;
}

/**
 * @brief Print character to framebuffer (x, y), where coords are used as
 *        symbol coordinates, not pixels
 */
void fb_overlay_put_char(int x, int y, char ch) {
	char const *data;
	uint8_t *dest;

	int bytes_per_pixel = text_fb->var.bits_per_pixel / 8;

	data = font->data + (unsigned char)ch * CHAR_HEIGHT * CHAR_WIDTH / 8;

	dest = overlay_sw_base + bytes_per_pixel * x * CHAR_WIDTH +
		bytes_per_pixel * y * CHAR_HEIGHT * text_fb->var.xres;

	for (int i = 0; i < CHAR_HEIGHT; i++) {
		for (int j = 0; j < CHAR_WIDTH; j++) {
 			if ((*data) & (1 << (CHAR_WIDTH - j  - 1)))
				memset(dest, 0xFF, bytes_per_pixel);
			else
				memset(dest, 0x00, bytes_per_pixel);
			dest += bytes_per_pixel;
		}
		data++;
		dest -= CHAR_WIDTH * bytes_per_pixel;
		dest += text_fb->var.xres * bytes_per_pixel;
	}
}

/**
 * @brief Print message to fb starting at (x, y), coords are passed as
 *        symbol numbers, not pixels
 */
void fb_overlay_put_string(int x, int y, char *str) {
	while (*str) {
		fb_overlay_put_char(x++, y, *str);
		str++;
	}
}

/**
 * @brief Same as fb_overlay_put_string(), but stops on '\n'
 */
void fb_overlay_put_line(int x, int y, char *str) {
	while (*str && *str != '\n') {
		fb_overlay_put_char(x++, y, *str);
		str++;
	}
}

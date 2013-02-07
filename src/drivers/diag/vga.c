/**
 * @file
 * @brief diag interface for framebuffer device & keyboard
 *
 * @date 09.09.11
 * @author Anton Kozlov
 */

#include <drivers/diag.h>
#include <drivers/keyboard.h>
#include <drivers/video/fb.h>
#include <drivers/video/font.h>

static char diag_vga_getc(void) {
	return keyboard_getc();
}

static void diag_vga_putc(char ch) {
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

static int diag_vga_kbhit(void) {
	return keyboard_has_symbol();
}

const struct diag_ops diag_vga_ops = {
	.getc = &diag_vga_getc,
	.putc = &diag_vga_putc,
	.kbhit = &diag_vga_kbhit
};

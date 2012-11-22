/**
 * @file
 * @brief XWnd support for text output
 *
 * @date Nov 7, 2012
 * @author Alexandr Chernakov
 */

#include <xwnd/fonts.h>
#include <xwnd/fonts/8x8.h>

extern void xwnd_print_char (const struct xwnd_window * wnd, unsigned x, unsigned y, char sym) {
	int tx, ty;
	unsigned char mask;
		for (ty = 0; ty < 8; ty++) {
	for (tx = 0; tx < 8; tx++) {
			mask = 0x01 << (7 - tx);
			if (xwnd_8x8_font[sym*8+ty] & mask)
				xwnd_draw_pixel(wnd, tx+x, ty+y, 2);
		}
	}
}

extern void xwnd_print_text (const struct xwnd_window * wnd, unsigned x, unsigned y, const char * text) {
	int i;
	for (i = 0; text[i]; i++) {
		xwnd_print_char(wnd, x+(i*8), y, text[i]);
	}
}

/**
 * @file
 * @brief XWnd drawing utilities
 *
 * @date Oct 10, 2012
 * @author Alexandr Chernakov
 */

#include <xwnd/draw_helpers.h>
#include <drivers/vesa.h>
#include <xwnd/xwnd.h>


void xwnd_draw_pixel (const struct xwnd_window * wnd, unsigned x, unsigned y, unsigned c) {
	if (x < wnd->wd && y < wnd->ht &&
			wnd->x + x >= 0 && wnd->x + x < vesa_get_width() &&
			wnd->y + y >= 0 && wnd->y + y < vesa_get_height())
	{
		vesa_put_pixel(wnd->x + x, wnd->y + y, c);
	}
}

static void xwnd_draw_horiz_line (unsigned x, unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i <= l; i++) {
		vesa_put_pixel(x + i, y, c);
	}
}

static void xwnd_draw_vert_line (unsigned x, unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i <= l; i++) {
		vesa_put_pixel(x, y + i, c);
	}
}

void xwnd_draw_window (struct xwnd_window * wnd) {
	xwnd_draw_vert_line  (wnd->x, wnd->y, wnd->ht, 2);
	xwnd_draw_horiz_line (wnd->x, wnd->y, wnd->wd, 2);
	xwnd_draw_vert_line  (wnd->x + wnd->wd, wnd->y, wnd->ht, 2);
	xwnd_draw_horiz_line (wnd->x, wnd->y + wnd->ht, wnd->wd, 2);
}

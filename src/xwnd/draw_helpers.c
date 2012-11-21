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
	if (x < wnd->wdg.rect.wd && y < wnd->wdg.rect.ht &&
			wnd->wdg.rect.x + x >= 0 && wnd->wdg.rect.x + x < vesa_get_width() &&
			wnd->wdg.rect.y + y >= 0 && wnd->wdg.rect.y + y < vesa_get_height())
	{
		vesa_put_pixel(wnd->wdg.rect.x + x, wnd->wdg.rect.y + y, c);
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
	xwnd_draw_vert_line  (wnd->wdg.rect.x, wnd->wdg.rect.y, wnd->wdg.rect.ht, 2);
	xwnd_draw_horiz_line (wnd->wdg.rect.x, wnd->wdg.rect.y, wnd->wdg.rect.wd, 2);
	xwnd_draw_vert_line  (wnd->wdg.rect.x + wnd->wdg.rect.wd, wnd->wdg.rect.y, wnd->wdg.rect.ht, 2);
	xwnd_draw_horiz_line (wnd->wdg.rect.x, wnd->wdg.rect.y + wnd->wdg.rect.ht, wnd->wdg.rect.wd, 2);
}

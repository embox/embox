#include <xwnd/draw_helpers.h>
#include <drivers/vesa.h>
#include <xwnd/xwnd.h>

void xwnd_draw_horiz_line (unsigned x, unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i <= l; i++) {
		vesa_put_pixel(x + i, y, c);
	}
}

void xwnd_draw_vert_line (unsigned x, unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i <= l; i++) {
		vesa_put_pixel(x, y + i, c);
	}
}

void xwnd_draw_window (struct xwnd_window * wnd) {
	xwnd_draw_vert_line  (wnd->wdg.x, wnd->wdg.y, wnd->wdg.ht, 2);
	xwnd_draw_horiz_line (wnd->wdg.x, wnd->wdg.y, wnd->wdg.wd, 2);
	xwnd_draw_vert_line  (wnd->wdg.x + wnd->wdg.wd, wnd->wdg.y, wnd->wdg.ht, 2);
	xwnd_draw_horiz_line (wnd->wdg.x, wnd->wdg.y + wnd->wdg.ht, wnd->wdg.wd, 2);
}

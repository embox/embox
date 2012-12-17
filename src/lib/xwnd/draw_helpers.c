/**
 * @file
 * @brief XWnd drawing utilities
 *
 * @date Oct 10, 2012
 * @author Alexandr Chernakov
 */

#include <lib/xwnd/draw_helpers.h>
#include <drivers/vesa.h>
#include <drivers/video/display.h>
#include <math.h>
#include <stdlib.h>

void xwnd_draw_pixel(const struct xwnd_window * wnd, unsigned x, unsigned y,
		unsigned c) {
	if ((x <= wnd->wd - 2) && (y <= wnd->ht - 2) &&
			(wnd->x + x + 1 < wnd->display->width) &&
			(wnd->x + x + 1 > 0) && (wnd->y + y + 1 < wnd->display->height)	&&
			(wnd->y + y + 1 > 0)) {
		display_set_pixel(wnd->display, wnd->x + x + 1, wnd->y + y + 1, c);
	}
}
#if 0
static void xwnd_draw_horiz_line(unsigned x, unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i < l; i++) {
		vesa_put_pixel(x + i, y, c);
	}
}

static void xwnd_draw_vert_line(unsigned x, unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i < l; i++) {
		vesa_put_pixel(x, y + i, c);
	}
}
#endif

//FIXME: horizontal/vertical line for windows, maybe it's necessary to rename it
void xwnd_draw_horizontal_line(const struct xwnd_window * wnd, unsigned x,
		unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i <= l; i++) {
		xwnd_draw_pixel(wnd, x + i, y, c);
	}
}

void xwnd_draw_vertical_line(const struct xwnd_window * wnd, unsigned x,
		unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i <= l; i++) {
		xwnd_draw_pixel(wnd, x, y + i, c);
	}
}

void xwnd_draw_line(const struct xwnd_window * wnd, unsigned x1, unsigned y1,
		unsigned x2, unsigned y2, unsigned c) {
	if (x1 == x2) {
		double t1, t2;
		t1 = min(y1, y2);
		t2 = max(y1, y2);
		xwnd_draw_vertical_line(wnd, x1, t1, t2 - t1, c);
		return;
	}

	if (y1 == y2) {
		double t1, t2;
		t1 = min(x1, x2);
		t2 = max(x1, x2);
		xwnd_draw_horizontal_line(wnd, t1, y1, t2 - t1, c);
		return;
	}

	{
		const int deltaX = abs(x2 - x1);
		const int deltaY = abs(y2 - y1);
		const int signX = x1 < x2 ? 1 : -1;
		const int signY = y1 < y2 ? 1 : -1;
		int error = deltaX - deltaY;
		xwnd_draw_pixel(wnd, x2, y2, c);
		while (x1 != x2 || y1 != y2) {
			const int error2 = error * 2;
			xwnd_draw_pixel(wnd, x1, y1, c);
			if (error2 > -deltaY) {
				error -= deltaY;
				x1 += signX;
			}
			if (error2 < deltaX) {
				error += deltaX;
				y1 += signY;
			}
		}
	}
}

void xwnd_draw_rectangle(const struct xwnd_window * wnd, unsigned x1,
		unsigned y1, unsigned x2, unsigned y2, unsigned c) {
	if ((x1 > x2) || (y1 > y2)) {
		return;
	}
	xwnd_draw_horizontal_line(wnd, x1, y1, x2 - x1, c);
	xwnd_draw_horizontal_line(wnd, x1, y2, x2 - x1, c);
	xwnd_draw_vertical_line(wnd, x1, y1, y2 - y1, c);
	xwnd_draw_vertical_line(wnd, x2, y1, y2 - y1, c);
}

void xwnd_draw_polygon(const struct xwnd_window * wnd, unsigned *points,
		int count, unsigned c) {
	int i;
	if (count % 2 != 0) {
		return;
	}
	for (i = 0; i < count - 2; i += 2) {
		xwnd_draw_line(wnd, points[i], points[i + 1], points[i + 2],
				points[i + 3], c);
	}
	xwnd_draw_line(wnd, points[i], points[i + 1], points[0], points[1], c);
}

void xwnd_draw_window(const struct xwnd_window * wnd) {
//	xwnd_draw_vert_line(wnd->x, wnd->y, wnd->ht, 2);
//	xwnd_draw_horiz_line(wnd->x, wnd->y, wnd->wd, 2);
//	xwnd_draw_vert_line(wnd->x + wnd->wd - 1, wnd->y, wnd->ht, 2);
//	xwnd_draw_horiz_line(wnd->x, wnd->y + wnd->ht - 1, wnd->wd, 2);
	xwnd_draw_rectangle(wnd, wnd->x, wnd->y, wnd->x + wnd->wd - 2, wnd->y + wnd->ht -2, 2);
}

void xwnd_clear_window(const struct xwnd_window * wnd) {
	int x, y;
	for (y = wnd->y + 1; y < wnd->y + wnd->ht - 1; y++) {
		for (x = wnd->x + 1; x < wnd->x + wnd->wd - 1; x++) {
			display_set_pixel(wnd->display, x, y, 0);
		}
	}
}

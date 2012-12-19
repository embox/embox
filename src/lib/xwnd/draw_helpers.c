/**
 * @file
 * @brief XWnd drawing utilities
 *
 * @date Oct 10, 2012
 * @author Alexandr Chernakov
 */

#include <lib/xwnd/draw_helpers.h>
//#include <drivers/vesa.h>
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

void xwnd_draw_ellipse(const struct xwnd_window * wnd, unsigned x, unsigned y,
		unsigned a, unsigned b, unsigned color) {
	int col, row;
	long a_square, b_square, two_a_square, two_b_square, four_a_square,
			four_b_square, d;
	double a_d, b_d;

	x += a;
	y += b;
	a_d = a / 2;
	b_d = b / 2;
	b_square = b_d * b_d;
	a_square = a_d * a_d;
	row = b_d;
	col = 0;
	two_a_square = a_square << 1;
	four_a_square = a_square << 2;
	four_b_square = b_square << 2;
	two_b_square = b_square << 1;
	d = two_a_square * ((row - 1) * (row)) + a_square
			+ two_b_square * (1 - a_square);
	while (a_square * (row) > b_square * (col)) {
		xwnd_draw_pixel(wnd, col + x, row + y, color);
		xwnd_draw_pixel(wnd, col + x, y - row, color);
		xwnd_draw_pixel(wnd, x - col, row + y, color);
		xwnd_draw_pixel(wnd, x - col, y - row, color);
		if (d >= 0) {
			row--;
			d -= four_a_square * (row);
		}
		d += two_b_square * (3 + (col << 1));
		col++;
	}
	d = two_b_square * (col + 1) * col + two_a_square * (row * (row - 2) + 1)
			+ (1 - two_a_square) * b_square;
	while ((row) + 1) {
		xwnd_draw_pixel(wnd, col + x, row + y, color);
		xwnd_draw_pixel(wnd, col + x, y - row, color);
		xwnd_draw_pixel(wnd, x - col, row + y, color);
		xwnd_draw_pixel(wnd, x - col, y - row, color);
		if (d <= 0) {
			col++;
			d += four_b_square * col;
		}
		row--;
		d += two_a_square * (3 - (row << 1));
	}
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

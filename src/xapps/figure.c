/**
 * @file
 * @brief XWnd draw figures application
 *
 * @date Nov 28, 2012
 * @author Vita Loginova
 */

#include <lib/xwnd/application.h>
#include <lib/xwnd/draw_helpers.h>
#include <lib/xwnd/fonts.h>
#include <math.h>
#include <stdlib.h>

void xwnd_draw_horiz_line(struct xwnd_application * xapp, unsigned x,
		unsigned y, unsigned l, unsigned c) {
	int i;
	for (i = 0; i <= l; i++) {
		xwnd_draw_pixel(&xapp->window, x + i, y, c);
	}
}

void xwnd_draw_vert_line(struct xwnd_application * xapp, unsigned x, unsigned y,
		unsigned l, unsigned c) {
	int i;
	for (i = 0; i <= l; i++) {
		xwnd_draw_pixel(&xapp->window, x, y + i, c);
	}
}

void xwnd_draw_line(struct xwnd_application * xapp, unsigned x1, unsigned y1,
		unsigned x2, unsigned y2, unsigned c) {
	int i;
	double a, b;

	if (x1 == x2) {
		double t1, t2;
		t1 = min(y1, y2);
		t2 = max(y1, y2);
		xwnd_draw_vert_line(xapp, x1, t1, t2 - t1, c);
		return;
	}

	if (y1 == y2) {
		double t1, t2;
		t1 = min(x1, x2);
		t2 = max(x1, x2);
		xwnd_draw_horiz_line(xapp, t1, y1, t2 - t1, c);
		return;
	}

	a = ((double) y2 - y1) / ((double) x2 - x1);
	b = (y1 - a * x1);
	if (abs((double) x2 - x1) > abs((double) y2 - y1)) {
		double t1, t2;
		t1 = min(x1, x2);
		t2 = max(x1, x2);
		for (i = t1; i <= t2; i++) {
			xwnd_draw_pixel(&xapp->window, i, a * i + b, c);
		}
	} else {
		double t1, t2;
		t1 = min(y1, y2);
		t2 = max(y1, y2);
		for (i = t1; i <= t2; i++) {
			xwnd_draw_pixel(&xapp->window, (i - b) / a, i, c);
		}
	}
}

void xwnd_draw_rectangle(struct xwnd_application * xapp, unsigned x1,
		unsigned y1, unsigned x2, unsigned y2, unsigned c) {
	if ((x1 > x2) || (y1 > y2)) {
		return;
	}
	xwnd_draw_horiz_line(xapp, xapp->window.x + x1, xapp->window.y + y1,
			x2 - x1, c);
	xwnd_draw_horiz_line(xapp, xapp->window.x + x1, xapp->window.y + y2,
			x2 - x1, c);
	xwnd_draw_vert_line(xapp, xapp->window.x + x1, xapp->window.y + y1, y2 - y1,
			c);
	xwnd_draw_vert_line(xapp, xapp->window.x + x2, xapp->window.y + y1, y2 - y1,
			c);
}

void xwnd_draw_polygon(struct xwnd_application * xapp, unsigned *points,
		int count, unsigned c) {
	int i;
	if (count % 2 != 0) {
		return;
	}
	for (i = 0; i < count - 2; i += 2) {
		xwnd_draw_line(xapp, points[i], points[i + 1], points[i + 2],
				points[i + 3], c);
	}
	xwnd_draw_line(xapp, points[i], points[i + 1], points[0], points[1], c);
}

void draw_rectangles(struct xwnd_application * xapp) {
	xwnd_draw_rectangle(xapp, 10, 10, 50, 50, 52);
	xwnd_draw_rectangle(xapp, 35, 20, 65, 40, 35);
	xwnd_draw_rectangle(xapp, 1, 30, 60, 90, 15);
	xwnd_draw_rectangle(xapp, 50, 45, 150, 69, 52);
	xwnd_draw_rectangle(xapp, 76, 32, 97, 40, 54);
	xwnd_draw_rectangle(xapp, 160, 40, 190, 70, 15);
}

void draw_lines(struct xwnd_application * xapp) {
	xwnd_draw_line(xapp, 155, 10, 175, 50, 52);
	xwnd_draw_line(xapp, 140, 12, 176, 97, 35);
	xwnd_draw_line(xapp, 100, 40, 170, 80, 15);
	xwnd_draw_line(xapp, 10, 250, 76, 32, 52);
	xwnd_draw_line(xapp, 38, 90, 65, 56, 35);
	xwnd_draw_line(xapp, 130, 43, 10, 190, 15);
}

void draw_polygons(struct xwnd_application * xapp) {
	unsigned points1[] = { 100, 100, 120, 120, 130, 80 };
	unsigned points2[] = { 10, 10, 50, 40, 80, 40, 120, 10 };
	unsigned points3[] = { 44, 50, 78, 90, 22, 60, 90, 44, 20, 36 };

	xwnd_draw_polygon(xapp, points1, 6, 35);
	xwnd_draw_polygon(xapp, points2, 8, 15);
	xwnd_draw_polygon(xapp, points3, 10, 52);
}

void clear(struct xwnd_application * xapp) {
	xwnd_clear_window(&xapp->window);
}

void print_help(struct xwnd_application * xapp) {
	xwnd_print_text(&xapp->window, 2, 2, "xfigure help:");
	xwnd_print_text(&xapp->window, 2, 12, "r - draw rectangles");
	xwnd_print_text(&xapp->window, 2, 22, "l - draw lines");
	xwnd_print_text(&xapp->window, 2, 32, "p - draw polygons");
	xwnd_print_text(&xapp->window, 2, 42, "c - clear");
	xwnd_print_text(&xapp->window, 2, 52, "h - help");
	xwnd_print_text(&xapp->window, 2, 62, "q - exit");
}

static void on_creat(struct xwnd_application * xapp, struct x_event * ev) {
	xapp->window.x = 1;
	xapp->window.y = 1;
	xapp->window.ht = 150;
	xapp->window.wd = 200;
	print_help(xapp);
}

static void on_draw(struct xwnd_application * xapp, struct x_event * ev) {
	xwnd_draw_window(&xapp->window);
}

static void on_quit(struct xwnd_application * xapp, struct x_event * ev) {
	xwnd_clear_window(&xapp->window);
}

static void on_key(struct xwnd_application * xapp, struct x_event * ev) {
	char key;
	key = ev->info.kbd.key;
	switch (key) {
	case 'r':
		clear(xapp);
		draw_rectangles(xapp);
		break;
	case 'l':
		clear(xapp);
		draw_lines(xapp);
		break;
	case 'p': {
		clear(xapp);
		draw_polygons(xapp);
		break;
	}
	case 'h':
		clear(xapp);
		print_help(xapp);
		break;
	case 'c':
		clear(xapp);
		break;
	}
}

static void * xwnd_figure_main(void * args) {
	struct xwnd_application xapp;
	xwnd_application_init(&xapp, NULL);

	xwnd_application_set_event_handler(&xapp, XEV_CREAT, on_creat);
	xwnd_application_set_event_handler(&xapp, XEV_DRAW, on_draw);
	xwnd_application_set_event_handler(&xapp, XEV_QUIT, on_quit);
	xwnd_application_set_event_handler(&xapp, XEV_KBD, on_key);

	xwnd_application_main_loop(&xapp);
	return NULL;
}

EMBOX_XWND_APP("xfigure", xwnd_figure_main);

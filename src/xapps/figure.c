/**
 * @file
 * @brief XWnd draw square application
 *
 * @date Nov 28, 2012
 * @author Vita Loginova
 */

#include <lib/xwnd/application.h>
#include <lib/xwnd/draw_helpers.h>
#include <lib/xwnd/fonts.h>
#include <math.h>

#include <xwnd/xappreg.h> /*Not usual include for XWndApplication, but it's
				nessesarry for terminal to launch other applications*/

static int length;
static int line;

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
	a = (double) (y2 - y1) / (x2 - x1);
	b = (y1 - a * x1);
	if (x2 - x1 > y2 - y1) {
		double t1, t2;
		t1 = min(x1, x2);
		t2 = max(x1, x2);
		for (i = t1; i <= t2; i++) {
			xwnd_draw_pixel(&xapp->window, i, a * i + b, c);
		}
	} else {
		int t1, t2;
		t1 = min(y1, y2);
		t2 = max(y1, y2);
		for (i = t1; i <= t2; i++) {
			xwnd_draw_pixel(&xapp->window, (double) (i - b) / a, i, c);
		}
	}
}

void xwnd_draw_rectangle(struct xwnd_application * xapp, unsigned x1,
		unsigned y1, unsigned x2, unsigned y2, unsigned c) {
	xwnd_draw_horiz_line(xapp, xapp->window.x + x1, xapp->window.y + y1,
			x2 - x1, c);
	xwnd_draw_horiz_line(xapp, xapp->window.x + x1, xapp->window.y + y2,
			x2 - x1, c);
	xwnd_draw_vert_line(xapp, xapp->window.x + x1, xapp->window.y + y1, y2 - y1,
			c);
	xwnd_draw_vert_line(xapp, xapp->window.x + x2, xapp->window.y + y1, y2 - y1,
			c);
}

static void on_creat(struct xwnd_application * xapp, struct x_event * ev) {

	length = 0;
	line = 0;

	xapp->window.x = 1;
	xapp->window.y = 1;
	xapp->window.ht = 150;
	xapp->window.wd = 200;
}

void clear(struct xwnd_application * xapp) {
	xwnd_clear_window(&xapp->window);
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
	case 's':
		xwnd_draw_rectangle(xapp, 10, 10, 50, 50, 52);
		xwnd_draw_rectangle(xapp, 35, 20, 65, 40, 52);
		xwnd_draw_rectangle(xapp, 1, 30, 60, 90, 52);
		break;
	case 'l':
		xwnd_draw_line(xapp, 155, 10, 175, 50, 15);
		xwnd_draw_line(xapp, 160, 12, 176, 97, 15);
		xwnd_draw_line(xapp, 160, 40, 168, 20, 15);
		break;
	case 'c':
		clear(xapp);
		break;
	}
}

static void * xwnd_term_main(void * args) {
	struct xwnd_application xapp;
	xwnd_application_init(&xapp, NULL);

	xwnd_application_set_event_handler(&xapp, XEV_CREAT, on_creat);
	xwnd_application_set_event_handler(&xapp, XEV_DRAW, on_draw);
	xwnd_application_set_event_handler(&xapp, XEV_QUIT, on_quit);
	xwnd_application_set_event_handler(&xapp, XEV_KBD, on_key);

	xwnd_application_main_loop(&xapp);
	return NULL;
}

EMBOX_XWND_APP("xfigure", xwnd_term_main);

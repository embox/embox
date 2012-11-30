/**
 * @file
 * @brief XWnd draw figures application
 *
 * @date Nov 28, 2012
 * @author Vita Loginova
 */

#include <lib/xwnd/application.h>
#include <lib/xwnd/resources.h>
#include <lib/xwnd/draw_helpers.h>
#include <lib/xwnd/fonts.h>

struct xfigure_res {
	void (*last_functon)(struct xwnd_application *);
};

XAPP_REGISTER_RES_TYPE(struct xfigure_res);

void draw_rectangles(struct xwnd_application * xapp) {
	xwnd_draw_rectangle(&xapp->window, 10, 10, 50, 50, 52);
	xwnd_draw_rectangle(&xapp->window, 35, 20, 65, 40, 35);
	xwnd_draw_rectangle(&xapp->window, 1, 30, 60, 90, 15);
	xwnd_draw_rectangle(&xapp->window, 50, 45, 150, 69, 52);
	xwnd_draw_rectangle(&xapp->window, 76, 32, 97, 40, 54);
	xwnd_draw_rectangle(&xapp->window, 160, 40, 190, 70, 15);
}

void draw_lines(struct xwnd_application * xapp) {
	xwnd_draw_line(&xapp->window, 155, 10, 175, 50, 52);
	xwnd_draw_line(&xapp->window, 140, 12, 176, 97, 35);
	xwnd_draw_line(&xapp->window, 100, 40, 170, 80, 15);
	xwnd_draw_line(&xapp->window, 10, 250, 76, 32, 52);
	xwnd_draw_line(&xapp->window, 38, 90, 65, 56, 35);
	xwnd_draw_line(&xapp->window, 130, 43, 10, 190, 15);
}

void draw_polygons(struct xwnd_application * xapp) {
	unsigned points1[] = { 100, 100, 120, 120, 130, 80 };
	unsigned points2[] = { 10, 10, 50, 40, 80, 40, 120, 10 };
	unsigned points3[] = { 44, 50, 78, 90, 22, 60, 90, 44, 20, 36 };

	xwnd_draw_polygon(&xapp->window, points1, 6, 35);
	xwnd_draw_polygon(&xapp->window, points2, 8, 15);
	xwnd_draw_polygon(&xapp->window, points3, 10, 52);
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
	struct xfigure_res * resources = XAPP_RESOURCES(xapp);
	xapp->window.x = 1;
	xapp->window.y = 1;
	xapp->window.ht = 150;
	xapp->window.wd = 200;
	resources->last_functon = print_help;
}

static void on_draw(struct xwnd_application * xapp, struct x_event * ev) {
	struct xfigure_res * resources = XAPP_RESOURCES(xapp);
	xwnd_clear_window(&xapp->window);
	xwnd_draw_window(&xapp->window);
	resources->last_functon(xapp);
}

static void on_quit(struct xwnd_application * xapp, struct x_event * ev) {
	xwnd_clear_window(&xapp->window);
}

static void on_key(struct xwnd_application * xapp, struct x_event * ev) {
	char key;
	struct xfigure_res * resources;

	resources = XAPP_RESOURCES(xapp);
	key = ev->info.kbd.key;
	switch (key) {
	case 'r':
		resources->last_functon = draw_rectangles;
		break;
	case 'l':
		resources->last_functon = draw_lines;
		break;
	case 'p': {
		resources->last_functon = draw_polygons;
		break;
	}
	case 'h':
		resources->last_functon = print_help;
		break;
	case 'c':
		resources->last_functon = clear;
		break;
	}
}

static void * xwnd_figure_main(void * args) {
	struct xwnd_application xapp;
	struct xfigure_res res;

	xwnd_application_init(&xapp, NULL);
	XAPP_INIT_RESOURCES (&xapp, res);

	xwnd_application_set_event_handler(&xapp, XEV_CREAT, on_creat);
	xwnd_application_set_event_handler(&xapp, XEV_DRAW, on_draw);
	xwnd_application_set_event_handler(&xapp, XEV_QUIT, on_quit);
	xwnd_application_set_event_handler(&xapp, XEV_KBD, on_key);

	xwnd_application_main_loop(&xapp);
	return NULL;
}

EMBOX_XWND_APP("xfigure", xwnd_figure_main);

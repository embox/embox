/**
 * @file
 * @brief XWnd terminal aplication
 *
 * @date Nov 7, 2012
 * @author Alexandr Chernakov
 */

#include <lib/xwnd/application.h>
#include <lib/xwnd/resources.h>
#include <lib/xwnd/draw_helpers.h>
#include <lib/xwnd/fonts.h>
#include <lib/xwnd/bmp.h>

struct ximg_res {
	struct xwnd_bmp_image * bmp;
};

XAPP_REGISTER_RES_TYPE(struct ximg_res);

static void on_creat (struct xwnd_application * xapp, struct x_event * ev) {
	struct ximg_res * resources = XAPP_RESOURCES(xapp);
	xwnd_application_place_window(xapp, 1);
	resources->bmp = xwnd_bmp_load("test2.bmp");
}

static void on_draw (struct xwnd_application * xapp, struct x_event * ev) {
	struct ximg_res * resources = XAPP_RESOURCES(xapp);
	xwnd_clear_window(&xapp->window);
	xwnd_draw_window(&xapp->window);
	xwnd_bmp_output(&xapp->window, resources->bmp);
}

static void on_quit (struct xwnd_application * xapp, struct x_event * ev) {
	struct ximg_res * resources = XAPP_RESOURCES(xapp);
	xwnd_bmp_unload(resources->bmp);
	xwnd_clear_window(&xapp->window);
}

static void on_key (struct xwnd_application * xapp, struct x_event * ev) {
}

static void * ximg_main(void * args) {
	struct xwnd_application xapp;
	struct ximg_res res;

	xwnd_application_init (&xapp, args);
	XAPP_INIT_RESOURCES (&xapp, res);

	xwnd_application_set_event_handler(&xapp, XEV_CREAT, on_creat);
	xwnd_application_set_event_handler(&xapp, XEV_DRAW, on_draw);
	xwnd_application_set_event_handler(&xapp, XEV_QUIT, on_quit);
	xwnd_application_set_event_handler(&xapp, XEV_KBD, on_key);

	xwnd_application_main_loop(&xapp);
	return NULL;
}

EMBOX_XWND_APP("ximg", ximg_main);

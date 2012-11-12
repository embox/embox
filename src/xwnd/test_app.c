/**
 * @file
 * @brief XWnd testing application conforming to desired standards
 *
 * @date Oct 18, 2012
 * @author Alexandr Chernakov
 */

#include <xwnd/xwnd.h>
#include <xwnd/test_app.h>
#include <drivers/vesa.h>

static struct xwnd_bmp_image * img;
static struct xwnd_application * xapp;

static void on_creat (struct xwnd_event * ev) {
	img = xwnd_bmp_load("test2.bmp");
}

static void on_draw (struct xwnd_event * ev) {
	//xwnd_bmp_output(xapp->wnd, img);
	//xwnd_print_char(xapp->wnd, 1, 1, 'A');
	xwnd_print_text(xapp->wnd, 1, 1, "@#%$!");
}

static void on_quit (struct xwnd_event * ev) {
	xwnd_bmp_unload(img);
	vesa_clear_screen();
}

static void on_key (struct xwnd_event * ev) {
	xwnd_draw_window(xapp->wnd);
}

void * test_app_main (void * args) {
	xapp = xwnd_app_init (args);
	xwnd_app_set_event_handle(xapp, XWND_EV_CREAT, on_creat);
	xwnd_app_set_event_handle(xapp, XWND_EV_DRAW, on_draw);
	xwnd_app_set_event_handle(xapp, XWND_EV_QUIT, on_quit);
	xwnd_app_set_event_handle(xapp, XWND_EV_KBD, on_key);
	xwnd_app_main_loop(xapp);
	xwnd_app_quit(xapp, 0);
	return NULL;
}

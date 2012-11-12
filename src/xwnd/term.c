/**
 * @file
 * @brief XWnd terminal aplication
 *
 * @date Nov 7, 2012
 * @author Alexandr Chernakov
 */

#include <xwnd/xwnd.h>
#include <xwnd/xwndapp.h>
#include <drivers/vesa.h>
#include <xwnd/app_registry.h>


static struct xwnd_application * xapp;
static char text_buf[1024];
static int length;
static int line;

static void on_creat (struct xwnd_event * ev) {
	text_buf[0] = '\0';
	length = 0;
	line = 0;

	/*xapp->wnd->wdg.rect.x = 101;
	xapp->wnd->wdg.rect.y = 1;
	xapp->wnd->wdg.rect.wd = 100;
	xapp->wnd->wdg.rect.ht = 100;*/
}

static void on_draw (struct xwnd_event * ev) {
	//vesa_clear_screen();
	xwnd_print_text(xapp->wnd, 1, 1 + line * 8, text_buf);
}

static void on_quit (struct xwnd_event * ev) {
	vesa_clear_screen();
}

static void on_key (struct xwnd_event * ev) {
	char key;
	key = ev->info.kbd.key;
	if (key == '\r') {
		text_buf[length = 0] = '\0';
		line++;
	} else if (key == 8) {
		text_buf[(length)?length--:length] = '\0';
	} else {
		text_buf[length] = key;
		text_buf[(length<1024)?++length:length] = '\0';
	}
}

static void * xwnd_term_main(void * args) {
	xapp = xwnd_app_init (args);
	xwnd_app_set_event_handle(xapp, XWND_EV_CREAT, on_creat);
	xwnd_app_set_event_handle(xapp, XWND_EV_DRAW, on_draw);
	xwnd_app_set_event_handle(xapp, XWND_EV_QUIT, on_quit);
	xwnd_app_set_event_handle(xapp, XWND_EV_KBD, on_key);
	xwnd_app_main_loop(xapp);
	xwnd_app_quit(xapp, 0);
	return NULL;
}

EMBOX_XWND_APP("xterm", xwnd_term_main);

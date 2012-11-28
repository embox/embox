/**
 * @file
 * @brief XWnd terminal aplication
 *
 * @date Nov 7, 2012
 * @author Alexandr Chernakov
 */

#include <lib/xwnd/application.h>
#include <lib/xwnd/draw_helpers.h>
#include <lib/xwnd/fonts.h>

#include <xwnd/xappreg.h> /*Not usual include for XWndApplication, but it's
				nessesarry for terminal to launch other applications*/

static char text_buf[1024];
static int length;
static int line;


static void execute (void) {
	int i;
	char command[1024], argument[20];
	for (i = 0; text_buf[i] != '\0' && text_buf[i] != ' '; i++) {
		command[i] = text_buf[i];
	}
	command[i] = '\0';
	argument[0] = argument[0];
	if (!text_buf[i]) {
		xwnd_app_start(command, NULL);
	} else if (text_buf[i+1] <= '3' && text_buf[i+1] >= '0') {
		argument[0] = text_buf[i+1];
		argument[1] = '\0';
		xwnd_app_start(command, argument);
	}
}

static void on_creat (struct xwnd_application * xapp, struct x_event * ev) {

	text_buf[0] = '\0';
	length = 0;
	line = 0;

	xapp->window.x = 1;
	xapp->window.y = 1;
	xapp->window.ht = 100;
	xapp->window.wd = 100;
}

static void on_draw (struct xwnd_application * xapp, struct x_event * ev) {
	xwnd_clear_window(&xapp->window);
	xwnd_draw_window(&xapp->window);
	xwnd_print_text(&xapp->window, 1, 1 + line * 8, text_buf);
}

static void on_quit (struct xwnd_application * xapp, struct x_event * ev) {
	xwnd_clear_window(&xapp->window);
}

static void on_key (struct xwnd_application * xapp, struct x_event * ev) {
	char key;
	key = ev->info.kbd.key;
	if (key == '\r') {
		execute();
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
	struct xwnd_application xapp;
	xwnd_application_init (&xapp);

	xwnd_application_set_event_handler(&xapp, XEV_CREAT, on_creat);
	xwnd_application_set_event_handler(&xapp, XEV_DRAW, on_draw);
	xwnd_application_set_event_handler(&xapp, XEV_QUIT, on_quit);
	xwnd_application_set_event_handler(&xapp, XEV_KBD, on_key);

	xwnd_application_main_loop(&xapp);
	return NULL;
}

EMBOX_XWND_APP("xterm", xwnd_term_main);

/*
 * xwndapp.c
 *
 *  Created on: 18.10.2012
 *      Author: alexandr
 */

#include <unistd.h> /*pipes()*/
#include <xwnd/xwndapp.h>
#include <xwnd/app_registry.h>
#include <xwnd/event.h>

/*static int xwnd_dispatch_event (void) {
	//int dispatched = 0;
	//struct xwnd_event event;
}*/

struct xwnd_application * xwnd_app_init (void) {
	return xwnd_app_create();
}

int xwnd_app_set_event_handle (void) {
	return 0;
}

int xwnd_app_main_loop (void) {
	return 0;
}

int xwnd_app_quit (int exit_status) {
	return 0;
}

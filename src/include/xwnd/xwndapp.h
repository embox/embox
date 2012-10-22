/*
 * xwndlib.h
 *
 *  Created on: 15.10.2012
 *      Author: alexandr
 */

#ifndef XWND_XWNDAPP_H_
#define XWND_XWNDAPP_H_

#include <xwnd/event.h>
#include <xwnd/window.h>

struct xwnd_application {
	struct xwnd_window * wnd;
	int pipe_in;
	int pipe_out;
	int app_id;
};

/**@brief Initialize the connection between application and XWnd*/
struct xwnd_application * xwnd_app_init (void);
/**@brief Connect a callback to an event*/
int xwnd_app_set_event_handle (void);
/**@brief Enter main window loop*/
int xwnd_app_main_loop (void);
int xwnd_app_quit (int exit_status);
#endif /* XWND_XWNDAPP_H_ */

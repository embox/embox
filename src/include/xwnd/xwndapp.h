/**
 * @file
 * @brief XWnd client-side API
 *
 * @date Oct 15, 2012
 * @author Alexandr Chernakov
 */

#ifndef XWND_XWNDAPP_H_
#define XWND_XWNDAPP_H_

#include <xwnd/event.h>
#include <xwnd/window.h>
#include <kernel/thread/sync/sem.h>

typedef void (*xwnd_event_handler) (struct xwnd_event * event);


struct xwnd_application {
	struct xwnd_window * wnd;
	int pipe_in;
	int pipe_out;
	int app_id;
	sem_t * msg_sem;
	sem_t * req_sem;
	xwnd_event_handler callbacks[XWND_EV_MAX];
};


extern int xwnd_app_set_event_handle (struct xwnd_application * app, enum xwnd_event_type ev_type, xwnd_event_handler handler);
extern int xwnd_app_get_event (struct xwnd_application * app, struct xwnd_event * event);
extern int xwnd_app_main_loop (struct xwnd_application * app);
extern struct xwnd_application * xwnd_app_init (void * args);
extern int xwnd_app_quit (struct xwnd_application * app, int exit_status);
#endif /* XWND_XWNDAPP_H_ */

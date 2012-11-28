/**
 * @file
 * @brief XWnd client-side API
 *
 * @date Nov 14, 2012
 * @author Alexandr Chernakov
 */

#ifndef LIB_XWND_APPLICATION_H_
#define LIB_XWND_APPLICATION_H_

#include <lib/x_event.h>
#include <lib/xwnd/window.h>
#include <xwnd/xappreg.h>

struct xwnd_application;

typedef void (*x_event_handler)(struct xwnd_application * app, struct x_event * event);

struct xwnd_application {
	int thread_id;
	int xapp_id;
	int argc;
	char ** argv;
	struct xwnd_window window;
	x_event_handler callbacks [XEV_MAX];
	struct xwnd_application_descriptor * descriptor;
	struct x_event_slave * xev_slave;
	void * resources;
};

struct xwnd_application_descriptor;

struct xwnd_application_args {
	int argc;
	char ** argv;
	struct xwnd_application_descriptor * descriptor;
};

extern struct xwnd_application * xwnd_application_struct_allocate (void);
extern void xwnd_application_struct_free (struct xwnd_application * app);

extern int xwnd_application_struct_init (struct xwnd_application * app, void * args);
extern void xwnd_application_struct_fini (struct xwnd_application * app);

extern int xwnd_application_subscribe (struct xwnd_application * app);
extern void xwnd_application_unsubscribe (struct xwnd_application * app);

extern void xwnd_application_get_args (struct xwnd_application * app, void * args);

extern void xwnd_application_place_window (struct xwnd_application * app, int quad);
extern int xwnd_application_init (struct xwnd_application * app, void * args);
extern void xwnd_applicationn_window_init (struct xwnd_application * app, int quad);
extern int xwnd_application_set_event_handler (struct xwnd_application * app, enum x_event_type, x_event_handler hndl);
extern void xwnd_application_drop_event_to_default_handler (struct xwnd_application * app, struct x_event * event);
extern int xwnd_application_main_loop (struct xwnd_application * app);

#endif

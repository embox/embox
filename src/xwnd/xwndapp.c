/*
 * xwndapp.c
 *
 *  Created on: 18.10.2012
 *      Author: alexandr
 */

#include <unistd.h>
#include <xwnd/xwndapp.h>
#include <xwnd/app_registry.h>
#include <xwnd/event.h>

static int xwnd_app_dispatch_event (const struct xwnd_application * app, struct xwnd_event * event) {
	if (app->callbacks[event->type]) {
		(app->callbacks[event->type])((void*)event);
	}
	return 0;
}


int xwnd_app_set_event_handle (struct xwnd_application * app, enum xwnd_event_type ev_type, xwnd_event_handler handler) {
	if (ev_type < 0 || ev_type >= XWND_EV_MAX)
		return 1;
	app->callbacks[ev_type] = handler;
	return 0;
}

int xwnd_app_get_event (struct xwnd_application * app, struct xwnd_event * event) {
	int err;
	sem_enter(&(app->msg_sem));
	err = read(app->pipe_in, event, sizeof(struct xwnd_event));
	sem_leave(&(app->msg_sem));
	if (err != sizeof(struct xwnd_event)) {
		return 1;
	}
	return 0;
}

int xwnd_app_main_loop (struct xwnd_application * app) {
	struct xwnd_event event;
	int exit_status;
	while (1) {
		int err;
		err = xwnd_app_get_event(app, &event);
		/*usleep(50);*/
		/*if (err) {
			exit_status = -1;
			break;
		}*/
		if (err == 0 && event.type == XWND_EV_QUIT) {
			exit_status = event.info.quit.exit_status;
			xwnd_app_dispatch_event(app, &event);
			break;
		}
		xwnd_app_dispatch_event(app, &event);
	}
	return exit_status;
}

struct xwnd_application * xwnd_app_init (void) {
	return xwnd_app_create();
}

int xwnd_app_quit (const struct xwnd_application * app, int exit_status) {
	return 0;
}

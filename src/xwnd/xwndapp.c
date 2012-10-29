/*
 * xwndapp.c
 *
 *  Created on: 18.10.2012
 *      Author: alexandr
 */

#include <unistd.h>
#include <stdlib.h>
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
	sem_enter(app->msg_sem);
	err = read(app->pipe_in, event, sizeof(struct xwnd_event));
	sem_leave(app->msg_sem);
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


int xwnd_app_reg_set_xapp_struct_ptr (void) {
	return 0;
}

struct xwnd_application * xwnd_app_init (void * args) {
	struct xwnd_application * t_xapp;
	struct xwnd_window * t_wnd;
	struct xwnd_rect rect;
	struct xwnd_app_init_wrapper * t_wrap = (struct xwnd_app_init_wrapper *)args;
	int xapp_id = t_wrap->xapp_id;

	t_xapp = malloc(sizeof(struct xwnd_application));
	if (!t_xapp) {
		return NULL;
	}

	//Create window
	rect.x = 30 * (xapp_id + 1);
	rect.y = 20 * (xapp_id + 1);
	rect.wd = 60 * (xapp_id + 1);
	rect.ht = 60 * (xapp_id + 1);
	t_wnd = xwnd_window_create(&rect); //malloc (sizeof(struct xwnd_window));
	if (!t_wnd) {
		free(t_xapp);
		return NULL;
	}
	t_xapp->wnd = t_wnd;
	t_xapp->app_id = xapp_id;
	t_xapp->req_sem = t_wrap->req_sem;
	t_xapp->msg_sem = t_wrap->msg_sem;
	t_xapp->pipe_out = t_wrap->req_pipe;
	t_xapp->pipe_in = t_wrap->msg_pipe;




	return t_xapp;
}

int xwnd_app_quit (struct xwnd_application * app, int exit_status) {
	free(app);
	return exit_status;
}

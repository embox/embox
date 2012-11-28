#include <lib/xwnd/application.h>

#include <stdlib.h>
#include <kernel/task.h>
#include <xwnd/xappreg.h>

static int xwnd_application_dispatch_event (struct xwnd_application * app, struct x_event * event) {
	if (app->callbacks[event->type]) {
		(app->callbacks[event->type])(app, (void*)event);
	}
	return 0;
}

struct xwnd_application * xwnd_application_struct_allocate (void) {
	return malloc(sizeof(struct xwnd_application));
}

void xwnd_application_struct_free (struct xwnd_application * app) {
	if (app)
		free (app);
}

int xwnd_application_struct_init (struct xwnd_application * app, void * args_ptr) {
	struct xwnd_application_args * args = (struct xwnd_application_args *) args_ptr;
	struct task * tmp_task;

	if (!args) {
		return 0;
	}

	tmp_task = task_self();
	app->thread_id = tmp_task->tid;
	app->descriptor = args->descriptor;
	app->argc = args->argc;
	app->argv = args->argv;

	return 1;
}

void xwnd_application_struct_fini (struct xwnd_application * app) {
}

int xwnd_application_subscribe (struct xwnd_application * app) {
	app->xapp_id = xwnd_app_registry_allocate();
	if (!xwnd_app_registry_get_subscription(app)) {
		xwnd_app_registry_free(app->xapp_id);
		return 0;
	}
	return 1;
}

void xwnd_application_unsubscribe (struct xwnd_application * app) {
	xwnd_app_registry_release_subscription(app->xapp_id);
}

void xwnd_applicationn_window_init (struct xwnd_application * app, int quad) {
}

int xwnd_application_fast_init (struct xwnd_application * app, int quad) {
	return 0;
}

int xwnd_application_set_event_handler (struct xwnd_application * app, enum x_event_type xev_type, x_event_handler hndl) {
	if (xev_type >= 0 && xev_type < XEV_MAX && app) {
		app->callbacks[xev_type] = hndl;
		return 1;
	}
	return 0;
}

void xwnd_application_drop_event_to_default_handler (struct xwnd_application * app, struct x_event * event) {
}

int xwnd_application_init (struct xwnd_application * app) {
	app->xapp_id = xwnd_app_registry_allocate();
	return xwnd_app_registry_get_subscription(app);
}

/*static void xwnd_app_place_window (int quad, struct xwnd_window * rect) {
	switch (quad) {
		case 0:
			rect->x = 0;
			rect->y = 0;
			rect->wd = 100;
			rect->ht = 100;
			break;
		case 1:
			rect->x = 100;
			rect->y = 0;
			rect->wd = 100;
			rect->ht = 100;
			break;
		case 2:
			rect->x = 0;
			rect->y = 100;
			rect->wd = 100;
			rect->ht = 100;
			break;
		case 3:
			rect->x = 100;
			rect->y = 100;
			rect->wd = 100;
			rect->ht = 100;
			break;
		default:
			break;
	}
}
*/
int xwnd_application_main_loop (struct xwnd_application * app) {
	struct x_event event;
	int exit_status;
	while (1) {
		int err;
		err = x_event_recv(app->xev_slave, &event);
		/*usleep(50);*/
		/*if (err) {
			exit_status = -1;
			break;
		}*/
		if (err == 1 && event.type == XEV_QUIT) {
			exit_status = event.info.quit.exit_status;
			xwnd_application_dispatch_event(app, &event);
			break;
		}
		xwnd_application_dispatch_event(app, &event);
	}
	return exit_status;
}

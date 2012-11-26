#include <xwnd/xappreg.h>

#include <embox/unit.h>
#include <util/array.h>
#include <kernel/task.h>
#include <string.h>

static struct xwnd_app_registry_node xwnd_app_nodes[MAX_XWND_APPS];
static int xwnd_focused_app;

ARRAY_SPREAD_DEF(const struct xwnd_app_desc, __xwnd_app_repository);
EMBOX_UNIT_INIT(xwnd_app_registry_init);

int xwnd_app_registry_init (void) {
	int i;
	for (i = 0; i < MAX_XWND_APPS; i++) {
		xwnd_app_nodes[i].empty = 1;
	}
	return 0;
}

void xwnd_app_registry_fini (void) {
}

const struct xwnd_app_desc *xwnd_app_desc_lookup(const char *app_name) {
	int i;

	for (i = 0; i < ARRAY_SPREAD_SIZE(__xwnd_app_repository); i++) {
		if(0 == strcmp(app_name, __xwnd_app_repository[i].app_name)) {
			return &__xwnd_app_repository[i];
		}
	}
	return NULL;
}

int xwnd_app_registry_allocate (void) {
	int i;
	for (i = 0; i < MAX_XWND_APPS; i++) {
		if (xwnd_app_nodes[i].empty) {
			xwnd_app_nodes[i].empty = 0;
			return i;
		}
	}
	return -1;
}

int xwnd_app_registry_move_focus (void) {
	int i;
	for (i = (xwnd_focused_app + 1) % MAX_XWND_APPS; xwnd_app_nodes[i].empty; i = (i + 1) % MAX_XWND_APPS);
	return xwnd_focused_app = i;
}

void xwnd_app_registry_free (int xapp_id) {
	if (xapp_id >= 0 && xapp_id < MAX_XWND_APPS) {
		xwnd_app_nodes[xapp_id].empty = 1;
	}
}

int xwnd_app_registry_get_subscription (struct xwnd_application * xapp) {
	if (xapp->xapp_id < 0 || xapp->xapp_id >= MAX_XWND_APPS) {
		return 0;
	}
	xwnd_app_nodes[xapp->xapp_id].xev_master = x_event_subscribe(xapp->xev_slave, XEVENG_MQUEUE);
	xwnd_app_nodes[xapp->xapp_id].thread_id = xapp->thread_id;
	xwnd_focused_app = xapp->xapp_id;
	return 1;
}

void xwnd_app_registry_release_subscription (int xapp_id) {
	x_event_unsubscribe(xwnd_app_nodes[xapp_id].xev_master);
}

void xwnd_app_registry_broadcast_event (struct x_event * ev) {
	int i;
	for (i = 0; i < MAX_XWND_APPS; i++) {
		if (!xwnd_app_nodes[i].empty)
			x_event_send(xwnd_app_nodes[i].xev_master, ev);
	}
}

int xwnd_app_registry_send_event (int xapp_id, struct x_event * ev) {
	return x_event_send(xwnd_app_nodes[xapp_id].xev_master, ev);
}

int xwnd_app_registry_send_event_focused (struct x_event * ev) {
	return xwnd_app_registry_send_event(xwnd_focused_app, ev);
}

int xwnd_app_start(const char *app_name, char *arg) {
	const struct xwnd_app_desc *app_desc;

	app_desc = xwnd_app_desc_lookup(app_name);
	if(NULL == app_desc) {
		return -1;
	}

	new_task(app_desc->run, (void *)arg);
	return 0;
}

void xwnd_app_registry_broadcast_quit_event (void) {
	struct x_event xev;
	xev.type = XEV_QUIT;
	xwnd_app_registry_broadcast_event(&xev);
}

void xwnd_app_registry_send_kbd_event (char key) {
	struct x_event xev;
	xev.type = XEV_KBD;
	xev.info.kbd.key = key;
	xwnd_app_registry_send_event_focused(&xev);
}

void xwnd_app_registry_send_sys_event (enum x_event_type type) {
	struct x_event xev;
	xev.type = type;
	xwnd_app_registry_send_event_focused(&xev);
}

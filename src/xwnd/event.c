/**
 * @file
 * @brief XWnd event-related routines implementation
 *
 * @date Oct 18, 2012
 * @author Alexandr Chernakov
 */

#include <stdlib.h>
#include <unistd.h>
#include <xwnd/event.h>
#include <xwnd/app_registry.h>
#include <kernel/thread/sync/sem.h>

int xwnd_app_send_event (int app_id, struct xwnd_event * event) {
	return xwnd_app_put_message_by_app_id (app_id, event, sizeof(struct xwnd_event));
}

int xwnd_event_send_event(int ev_id, struct xwnd_event * event) {
	return xwnd_app_put_message_by_event_id (ev_id, event, sizeof(struct xwnd_event));
}

int xwnd_app_send_quit_event (int app_id, int exit_status) {
	struct xwnd_event ev;
	ev.type = XWND_EV_QUIT;
	ev.info.quit.exit_status = exit_status;
	return xwnd_app_send_event(app_id, &ev);
}

int xwnd_app_send_kbd_event(int app_id, char key) {
	struct xwnd_event ev;
	ev.type = XWND_EV_KBD;
	ev.info.kbd.key = key;
	return xwnd_app_send_event(app_id, &ev);
}

int xwnd_app_send_sys_event(int app_id, enum xwnd_event_type event_type) {
	struct xwnd_event ev;
	ev.type = event_type;
	return xwnd_app_send_event(app_id, &ev);
}

int xwnd_event_create_pair(struct xwnd_event_master * master, struct xwnd_event_slave * slave) {
	int req_pipe[2], msg_pipe[2];
	sem_init(&(master->msg_sem), 2);
	sem_init(&(master->req_sem), 2);
	slave->msg_sem = &(master->msg_sem);
	slave->req_sem = &(master->req_sem);
	if (pipe(req_pipe)) {
	slave->msg_sem = &(master->msg_sem);
		return -1;
	}
	if (pipe(msg_pipe)) {
		/*close(req_pipe[0]);
		close(req_pipe[1]);*/
		return -1;
	}
	master->msg_pipe = msg_pipe[1];
	master->req_pipe = req_pipe[0];
	slave->msg_pipe = msg_pipe[0];
	slave->req_pipe = req_pipe[1];
	master->active = 1;
	return 0;
}

void xwnd_event_remove_pair(struct xwnd_event_master * master, struct xwnd_event_slave * slave) {
	master->active = 0;
}

struct xwnd_event_supervisor * xwnd_event_init_supervisor(int reserve) {
	int i;
	struct xwnd_event_supervisor * sup = NULL;
	sup = malloc (sizeof(struct xwnd_event_supervisor));
	if (!sup) {
		return NULL;
	}
	sup->masters = malloc (reserve * sizeof(struct xwnd_event_master));
	if (!sup->masters) {
		free(sup);
		return NULL;
	}
	for (i = 0; i < reserve; i++) {
		sup->masters[i].active = 0;
	}
	sup->allocated = reserve;
	return sup;
}

void xwnd_event_quit_supervisor(struct xwnd_event_supervisor * sup) {
	if (sup) {
		if (sup->masters)
			free(sup->masters);
		free(sup);
	}
}

int xwnd_event_move_focus (struct xwnd_event_supervisor * sup) {
	int i;
	for (i = ((sup->focus + 1)%sup->allocated); !sup->masters[i].active; i = ((i+1)%sup->allocated));
	sup->focus = i;
	return i;
}

int xwnd_event_get_focus (struct xwnd_event_supervisor * sup) {
	return sup->focus;
}

/** @return Retruns xwnd_event_master struct ID, or -1 on error*/
int xwnd_event_get_supervised_pair(struct xwnd_event_supervisor * sup, struct xwnd_event_slave * slave) {
	int i;
	for (i = 0; i < sup->allocated; i++) {
		if (sup->masters[i].active == 0) {
			xwnd_event_create_pair(&(sup->masters[i]), slave);
			sup->focus = i;
			return i;
		}
	}
	return -1;
}

void xwnd_event_free_supervised_pair(struct xwnd_event_supervisor * sup, int id) {
	/*ololo*/
}

int xwnd_event_direct_event (struct xwnd_event_supervisor * sup, struct xwnd_event * event) {
	return xwnd_event_send_event(sup->focus, event);
}

int xwnd_event_broadcast_event (struct xwnd_event_supervisor * sup, struct xwnd_event * event) {
	int err, i;
	for (i = 0; i < sup->allocated; i++) {
		if (sup->masters[i].active) {
			err = xwnd_event_send_event(i, event);
		}
	}
	return err;
}

int xwnd_event_broadcast_quit_event (struct xwnd_event_supervisor * sup, int exit_status) {
	struct xwnd_event ev;
	ev.type = XWND_EV_QUIT;
	ev.info.quit.exit_status = exit_status;
	return xwnd_event_broadcast_event(sup, &ev);
}

int xwnd_event_send_kbd_event(struct xwnd_event_supervisor * sup, char key) {
	struct xwnd_event ev;
	ev.type = XWND_EV_KBD;
	ev.info.kbd.key = key;
	return xwnd_event_direct_event(sup, &ev);
}

int xwnd_event_send_sys_event(struct xwnd_event_supervisor * sup, enum xwnd_event_type event_type) {
	struct xwnd_event ev;
	ev.type = event_type;
	return xwnd_event_direct_event(sup, &ev);
}

int xwnd_event_broadcast_sys_event(struct xwnd_event_supervisor * sup, enum xwnd_event_type event_type) {
	struct xwnd_event ev;
	ev.type = event_type;
	return xwnd_event_broadcast_event(sup, &ev);
}


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
	return xwnd_app_put_message (app_id, event, sizeof(struct xwnd_event));
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
	return 0;
}

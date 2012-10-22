/*
 * event.h
 *
 *  Created on: 15.10.2012
 *      Author: alexandr
 */

#ifndef XWND_EVENT_H_
#define XWND_EVENT_H_

#include <kernel/thread/event.h>
#include <xwnd/app_registry.h>

enum xwnd_event_type {XWND_CREAT, XWND_SIGNALLED, XWND_KILLED,
	XWND_KBD_PRESS, XWND_DRAW, XWND_EVENT_MAX /*to be continued*/};
struct xwnd_kbd_event {
	char key; /*Oh, forgive me, mom*/
};
struct xwnd_mouse_event {
	int x;
	int y;
	int state;
};
struct xwnd_quit_event {
	int exit_status;
};
union xwnd_event_info {
	struct xwnd_kbd_event kbd;
	struct xwnd_mouse_event mouse;
	struct xwnd_quit_event quit;
};
struct xwnd_event {
	enum xwnd_event_type type;
	struct event krnl_event;
	union xwnd_event_info info;
};
typedef void (*xwnd_event_handler) (struct xwnd_event * event);

int xwnd_send_event (int app_id, const struct xwnd_event * event);


#endif /* XWND_EVENT_H_ */
